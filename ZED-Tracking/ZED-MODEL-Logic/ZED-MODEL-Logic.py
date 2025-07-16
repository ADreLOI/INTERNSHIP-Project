import socket
import json
import math
import cv2
import pyzed.sl as sl

# ——— UDP setup —————————————————————————————
UDP_IP   = "10.196.91.47"   # Indirizzo della macchina UE
UDP_PORT = 5005
sock     = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# ——— ZED + Body Tracking setup ———————————————————
zed = sl.Camera()
init = sl.InitParameters()
init.camera_resolution = sl.RESOLUTION.HD720
init.depth_mode       = sl.DEPTH_MODE.PERFORMANCE

if zed.open(init) != sl.ERROR_CODE.SUCCESS:
    print("ZED Open Error"); exit(1)

bt_params = sl.BodyTrackingParameters()
bt_params.enable_body_fitting = True
if zed.enable_body_tracking(bt_params) != sl.ERROR_CODE.SUCCESS:
    print("Failed to enable Body Tracking"); exit(1)

runtime = sl.RuntimeParameters()
bodies  = sl.Bodies()

# ——— Utility: quaternion → roll angle in gradi —————————————————
def quat_to_roll(q):
    # q = (x,y,z,w)
    x, y, z, w = q
    sinr = 2.0 * (w*x + y*z)
    cosr = 1.0 - 2.0 * (x*x + y*y)
    return math.degrees(math.atan2(sinr, cosr))

print("ZED Body Tracking → sending UDP [position + angle]. Press 'q' to quit.")

while True:
    if zed.grab(runtime) == sl.ERROR_CODE.SUCCESS:
        # recupera liste di corpi
        zed.retrieve_bodies(bodies)

        msg = {"left_wrist": None, "right_wrist": None}

        for body in bodies.body_list:
            # LEFT WRIST
            jL = body.keypoint[sl.BODY_PART.LEFT_WRIST]
            pL = jL.position    # Vector4 [X,Y,Z,1]
            qL = jL.orientation # Quaternion x,y,z,w
            rollL = quat_to_roll((qL[0],qL[1],qL[2],qL[3]))

            msg["left_wrist"] = {
                "position": {"x": pL[0], "y": pL[1], "z": pL[2]},
                "angle": rollL
            }

            # RIGHT WRIST
            jR = body.keypoint[sl.BODY_PART.RIGHT_WRIST]
            pR = jR.position
            qR = jR.orientation
            rollR = quat_to_roll((qR[0],qR[1],qR[2],qR[3]))

            msg["right_wrist"] = {
                "position": {"x": pR[0], "y": pR[1], "z": pR[2]},
                "angle": rollR
            }

            # Se hai più corpi, commenta il break per inviarli tutti
            break

        # invia via UDP
        sock.sendto(json.dumps(msg).encode('utf-8'), (UDP_IP, UDP_PORT))

        # ——— Debug overlay —————————————————————————
        # recupera e mostra l'immagine per feedback
        img = sl.Mat()
        zed.retrieve_image(img, sl.VIEW.LEFT)
        frame = cv2.cvtColor(img.get_data(), cv2.COLOR_RGBA2BGR)

        # scrivi posizione e roll sui polsi
        if msg["left_wrist"] is not None:
            x,y,z = msg["left_wrist"]["position"].values()
            a     = msg["left_wrist"]["angle"]
            cv2.putText(frame,
                        f"L: ({x:.2f},{y:.2f},{z:.2f}) roll={a:.1f}",
                        (10, 30),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,255,0), 1)

        if msg["right_wrist"] is not None:
            x,y,z = msg["right_wrist"]["position"].values()
            a     = msg["right_wrist"]["angle"]
            cv2.putText(frame,
                        f"R: ({x:.2f},{y:.2f},{z:.2f}) roll={a:.1f}",
                        (10, 50),
                        cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,0,255), 1)

        cv2.imshow("ZED Body Tracking Debug", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

# cleanup
zed.close()
cv2.destroyAllWindows()
sock.close()
