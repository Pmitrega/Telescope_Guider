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
sec_per_pixel = 6.35
solver = None

def connect_mqtt():
    def on_connect(client, userdata, flags, rc, properties):
    # For paho-mqtt 2.0.0, you need to add the properties parameter.
    # def on_connect(client, userdata, flags, rc, properties):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)
    # Set Connecting Client ID
    # client = mqtt_client.Client(client_id)

    # For paho-mqtt 2.0.0, you need to set callback_api_version.
    client = mqtt_client.Client(client_id=client_id, callback_api_version=mqtt_client.CallbackAPIVersion.VERSION2)

    # client.username_pw_set(username, password)
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
    deg = math.floor(degrees)
    mts = (degrees - deg)*60
    mts_round = math.floor(mts)
    secs = round((mts - mts_round)* 60, 1)
    return "["+str(deg) + "," + str(mts_round)  + "," + str(secs) +  "]"

def deg_to_hms(degrees):
    hrs = degrees/15
    hrs_round = math.floor(hrs)
    mts = (hrs - hrs_round)*60
    mts_round = math.floor(mts)
    secs = round((mts - mts_round)* 60, 1)
    return "["+str(hrs_round) + "," + str(mts_round)  + "," + str(secs) +  "]"


def on_message(client, userdata, msg):
    if msg.topic == "solver/star_locs":
        print("solving field ...")
        stars = msg.payload.decode("utf-8")
        stars = json.loads(stars)
        print(stars)
        if sec_per_pixel is not None:
            solution = solver.solve(
                stars=stars,
                size_hint=astrometry.SizeHint(
                            lower_arcsec_per_pixel=sec_per_pixel - 0.1,
                            upper_arcsec_per_pixel=sec_per_pixel + 0.1,
                ),
                    position_hint=None,
                solution_parameters=astrometry.SolutionParameters(
                    positional_noise_pixels= 1.0,
                    output_logodds_threshold = math.log(1e7),
                    tune_up_logodds_threshold = math.log(1e4),
                    distractor_ratio = 0.1
                ))
        else:
            solution = solver.solve(
                stars=stars,
                size_hint=astrometry.SizeHint(
                            lower_arcsec_per_pixel=sec_per_pixel - 0.5,
                            upper_arcsec_per_pixel=sec_per_pixel + 0.5,
                ),
                    position_hint=astrometry.PositionHint(
                    ra_deg=100.0,
                    dec_deg=69.0,
                    radius_deg=5.0,
                ),
                solution_parameters=astrometry.SolutionParameters(
                    positional_noise_pixels=2.0,
                    minimum_quad_size_pixels = 100,
                    distractor_ratio = 0.1

                ))
        if solution.has_match():
            print(f"{solution.best_match().center_ra_deg=}")
            print(f"{solution.best_match().center_dec_deg=}")
            print(f"{solution.best_match().scale_arcsec_per_pixel=}")
            M1 = solution.best_match().wcs_fields["CD1_1"][0]
            M3 = solution.best_match().wcs_fields["CD2_1"][0]
            # print(solution.best_match().wcs_fields["CD1_1"])
            # print(solution.best_match().wcs_fields["CD1_2"])
            # print(solution.best_match().wcs_fields["CD2_1"])
            # print(solution.best_match().wcs_fields["CD2_2"])
            print(M1)
            print(M3)
            rot = round(abs(math.atan2(M1, M3) * 180/math.pi) + 270, 2)%360
            print("Rotation: ", rot)
            client.publish("solver/ra_deg", str(solution.best_match().center_ra_deg))
            client.publish("solver/ra_hms", deg_to_hms(solution.best_match().center_ra_deg))
            client.publish("solver/dec_deg", str(solution.best_match().center_dec_deg))
            client.publish("solver/dec_dms", deg_to_dms(solution.best_match().center_dec_deg))
            client.publish("solver/rotation", str(round(rot, 4)))
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


