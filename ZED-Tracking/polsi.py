import cv2
import mediapipe as mp
import numpy as np
import pyzed.sl as sl
import socket
import json

# ——— UDP setup —————————————————————————————
UDP_IP = "127.0.0.1"
UDP_PORT = 5005
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

# ——— ZED setup —————————————————————————————
zed = sl.Camera()
init_params = sl.InitParameters()
init_params.camera_resolution = sl.RESOLUTION.HD720
init_params.depth_mode = sl.DEPTH_MODE.PERFORMANCE
if zed.open(init_params) != sl.ERROR_CODE.SUCCESS:
    print("ZED Open Error")
    exit(1)
image_zed   = sl.Mat()
point_cloud = sl.Mat()
runtime_params = sl.RuntimeParameters()

# ——— MediaPipe Pose setup ————————————————————
mp_pose    = mp.solutions.pose
mp_drawing = mp.solutions.drawing_utils
pose = mp_pose.Pose(static_image_mode=False,
                    model_complexity=1,
                    enable_segmentation=False,
                    min_detection_confidence=0.5,
                    min_tracking_confidence=0.5)

print("Running wrist orientation tracking... Press 'q' to quit.")

while True:
    if zed.grab(runtime_params) == sl.ERROR_CODE.SUCCESS:
        zed.retrieve_image(image_zed, sl.VIEW.LEFT)
        zed.retrieve_measure(point_cloud, sl.MEASURE.XYZ)

        frame_rgba = image_zed.get_data()
        frame = cv2.cvtColor(frame_rgba, cv2.COLOR_RGBA2BGR)

        results = pose.process(frame)
        message = {"left_wrist": None, "right_wrist": None}

        if results.pose_landmarks:
            mp_drawing.draw_landmarks(frame, results.pose_landmarks, mp_pose.POSE_CONNECTIONS)
            landmarks = results.pose_landmarks.landmark
            h, w, _ = frame.shape

            def process_wrist(wrist_id, pinky_id, index_id, label):
                wrist = landmarks[wrist_id]
                pinky = landmarks[pinky_id]
                index = landmarks[index_id]

                cx, cy = int(wrist.x * w), int(wrist.y * h)
                point3d = point_cloud.get_value(cx, cy)[1]
                wrist_pt = np.array(point3d)[:3]

                ix, iy = int(index.x * w), int(index.y * h)
                index_pt = np.array(point_cloud.get_value(ix, iy)[1])[:3]

                ex, ey = int(pinky.x * w), int(pinky.y * h)
                pinky_pt = np.array(point_cloud.get_value(ex, ey)[1])[:3]

                v1 = index_pt - wrist_pt
                v2 = pinky_pt - wrist_pt
                normal = np.cross(v1, v2)
                norm = np.linalg.norm(normal)
                if norm != 0:
                    normal = normal / norm
                angle = np.degrees(np.arccos(np.clip(abs(normal[1]), -1.0, 1.0)))
                orientation = "orizzontale" if angle < 45 else "verticale"

                cv2.putText(frame,
                            f"{label}: {orientation}",
                            (cx, cy),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5, (0, 255, 0), 1)

                message[label] = {
                    "position": {
                        "x": float(wrist_pt[0]),
                        "y": float(wrist_pt[1]),
                        "z": float(wrist_pt[2])
                    },
                    "orientation": orientation
                }
                return wrist_pt

            left_pt = process_wrist(mp_pose.PoseLandmark.LEFT_WRIST.value,
                                    mp_pose.PoseLandmark.LEFT_ELBOW.value,
                                    mp_pose.PoseLandmark.LEFT_INDEX.value,
                                    "left_wrist")

            right_pt = process_wrist(mp_pose.PoseLandmark.RIGHT_WRIST.value,
                                     mp_pose.PoseLandmark.RIGHT_ELBOW.value,
                                     mp_pose.PoseLandmark.RIGHT_INDEX.value,
                                     "right_wrist")

            if left_pt is not None:
                cv2.putText(frame,
                            f"Polso Sinistro: {left_pt[0]:.2f}, {left_pt[1]:.2f}, {left_pt[2]:.2f}",
                            (10, 20),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5, (255, 0, 0), 1)

            if right_pt is not None:
                cv2.putText(frame,
                            f"Polso Destro: {right_pt[0]:.2f}, {right_pt[1]:.2f}, {right_pt[2]:.2f}",
                            (10, 40),
                            cv2.FONT_HERSHEY_SIMPLEX,
                            0.5, (255, 0, 0), 1)

        sock.sendto(json.dumps(message).encode('utf-8'), (UDP_IP, UDP_PORT))

        cv2.imshow("ZED Wrist Orientation", frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

cv2.destroyAllWindows()
pose.close()
zed.close()
sock.close()