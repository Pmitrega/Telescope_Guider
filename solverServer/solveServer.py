from paho.mqtt import client as mqtt_client
import json
import astrometry
import math

broker = 'localhost'
port = 1883
topic = "solver/solution"
client_id = f'solver'
username = 'stepper'
password = 'stepper'
sec_per_pixel = 6.35/2
solver = None

def connect_mqtt():
    def on_connect(client, userdata, flags, rc, properties):
        # For paho-mqtt 2.0.0, you need to add the properties parameter.
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    # Set Connecting Client ID
    client = mqtt_client.Client(client_id=client_id, callback_api_version=mqtt_client.CallbackAPIVersion.VERSION2)
    client.on_connect = on_connect
    client.on_message = on_message
    client.connect(broker, port)
    client.subscribe("solver/star_locs")
    client.subscribe("solver/sec_per_pixel")

    return client

FIRST_RECONNECT_DELAY = 1
RECONNECT_RATE = 2
MAX_RECONNECT_COUNT = 12
MAX_RECONNECT_DELAY = 60

def on_disconnect(client, userdata, rc):
    logging.info("Disconnected with result code: %s", rc)
    reconnect_count, reconnect_delay = 0, FIRST_RECONNECT_DELAY
    while reconnect_count < MAX_RECONNECT_COUNT:
        logging.info("Reconnecting in %d seconds...", reconnect_delay)
        time.sleep(reconnect_delay)

        try:
            client.reconnect()
            logging.info("Reconnected successfully!")
            return
        except Exception as err:
            logging.error("%s. Reconnect failed. Retrying...", err)

        reconnect_delay *= RECONNECT_RATE
        reconnect_delay = min(reconnect_delay, MAX_RECONNECT_DELAY)
        reconnect_count += 1
    logging.info("Reconnect failed after %s attempts. Exiting...", reconnect_count)

def deg_to_dms(degrees):
    deg = math.floor(degrees)  # Integer part of degrees
    mts = (degrees - deg) * 60  # Get remaining minutes
    mts_round = math.floor(mts)  # Integer part of minutes
    secs = (mts - mts_round) * 60  # Remaining seconds
    
    # Round seconds to 1 decimal place
    secs_round = round(secs, 1)

    return f"[{deg},{mts_round},{secs_round}]"

def deg_to_hms(degrees):
    # Convert degrees to hours (1 hour = 15 degrees)
    hrs = degrees / 15
    hrs_round = math.floor(hrs)  # Integer part of hours
    mts = (hrs - hrs_round) * 60  # Get remaining minutes
    mts_round = math.floor(mts)  # Integer part of minutes
    secs = (mts - mts_round) * 60  # Remaining seconds
    
    # Round seconds to 1 decimal place
    secs_round = round(secs, 1)

    return f"[{hrs_round},{mts_round},{secs_round}]"

def on_message(client, userdata, msg):
    if msg.topic == "solver/star_locs":
        print("solving field ...")
        stars = msg.payload.decode("utf-8")
        stars = json.loads(stars)
        stars = [[y, x] for x, y in stars]  # Correct indentation here
        print(stars)
        if sec_per_pixel is not None:
            print(sec_per_pixel)
            solution = solver.solve(
                stars=stars,
                size_hint=astrometry.SizeHint(
                    lower_arcsec_per_pixel=sec_per_pixel - 1,
                    upper_arcsec_per_pixel=sec_per_pixel + 1,
                ),
                position_hint=None,
                solution_parameters=astrometry.SolutionParameters(
                    positional_noise_pixels=1.0,
                    output_logodds_threshold=math.log(1e7),
                    tune_up_logodds_threshold=math.log(1e4),
                    distractor_ratio=0.1
                ))
        else:
            solution = solver.solve(
                stars=stars,
                size_hint=astrometry.SizeHint(
                    lower_arcsec_per_pixel=sec_per_pixel - 0.5,
                    upper_arcsec_per_pixel=sec_per_pixel + 0.5,
                ),
                solution_parameters=astrometry.SolutionParameters(
                    positional_noise_pixels=2.0,
                    minimum_quad_size_pixels=100,
                    distractor_ratio=0.1
                ))
        if solution.has_match():
            wcs_fields = solution.best_match().wcs_fields

            CRPIX1 = float(wcs_fields["CRPIX1"][0])
            CRPIX2 = float(wcs_fields["CRPIX2"][0])
            CRVAL1 = float(wcs_fields["CRVAL1"][0])  # RA at ref pixel
            CRVAL2 = float(wcs_fields["CRVAL2"][0])  # Dec at ref pixel
            CD11 = float(wcs_fields["CD1_1"][0])
            CD12 = float(wcs_fields["CD1_2"][0])
            CD21 = float(wcs_fields["CD2_1"][0])
            CD22 = float(wcs_fields["CD2_2"][0])
            # Image center pixel (Python uses 0-indexed, FITS often 1-indexed)
            x = 640
            y = 480
            dx = x + 1 - CRPIX1
            dy = y + 1 - CRPIX2
            print("crpx:",CRPIX1, CRPIX2)
            print("CD11, CD12", CD11, CD12)
            print("CD21, CD22", CD21, CD22)
            print(CRVAL1, CRVAL2)
            # Linear WCS transform
            print((1/math.cos(CRVAL2 * math.pi/180)))
            ra_center = CRVAL1 + CD11 * dx* (1/math.cos(CRVAL2 * math.pi/180)) + CD12 * dy* (1/math.cos(CRVAL2 * math.pi/180))
            dec_center = CRVAL2 + CD21 * dx + CD22 * dy
            print(f"{solution.best_match().center_ra_deg=}")
            print(f"{solution.best_match().center_dec_deg=}")
            print(f"{solution.best_match().scale_arcsec_per_pixel=}")
            M2 = solution.best_match().wcs_fields["CD1_2"][0]
            M1 = solution.best_match().wcs_fields["CD1_1"][0]
            M3 = solution.best_match().wcs_fields["CD2_1"][0]
            M4 = solution.best_match().wcs_fields["CD2_2"][0]
            WCS = "[" + str(M1) + "," + str(M2) + "," + str(M3) + "," + str(M4) + ","+ str(CRPIX1) + ","+ str(CRPIX2) + ","+ str(CRVAL1) + ","+ str(CRVAL2) + "]"
            print(M1)
            print(M3)
            rot = round(abs(math.atan2(M1, M3) * 180 / math.pi) + 270, 2) % 360
            print("Rotation: ", rot)
            client.publish("solver/ra_deg", str(ra_center))
            client.publish("solver/ra_hms", deg_to_hms(ra_center))
            client.publish("solver/dec_deg", str(dec_center))
            client.publish("solver/dec_dms", deg_to_dms(dec_center))
            client.publish("solver/rotation", str(round(rot, 4)))
            client.publish("solver/wcs", WCS)
        else:
            print(f"solution not found")
    if msg.topic == "solver/sec_per_pixel":
        spc = msg.payload.decode("utf-8")
        spc = float(spc)

if __name__ == "__main__":
    print("Preparing solver ...")
    solver = astrometry.Solver(
        astrometry.series_4100.index_files(
            cache_directory="astrometry_cache",
            scales={8}))
    print("Solver prepared ...")
    client = connect_mqtt()

    client.loop_forever()