import cv2
import mediapipe as mp
import numpy as np
import pyzed.sl as sl

# Setup ZED camera
zed = sl.Camera()
init_params = sl.InitParameters()
init_params.camera_resolution = sl.RESOLUTION.HD720
init_params.depth_mode = sl.DEPTH_MODE.PERFORMANCE
status = zed.open(init_params)

if status != sl.ERROR_CODE.SUCCESS:
    print(f"ZED Open Error: {status}")
    exit()

# Create objects to hold images
time_stamp = sl.Timestamp()
image_zed = sl.Mat()
point_cloud = sl.Mat()

# Setup MediaPipe Hands
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(static_image_mode=False, max_num_hands=1, min_detection_confidence=0.7, min_tracking_confidence=0.5)
mp_drawing = mp.solutions.drawing_utils

runtime_params = sl.RuntimeParameters()


print("Running hand tracking with ZED... Press 'q' to quit.")

while True:
    if zed.grab(runtime_params) == sl.ERROR_CODE.SUCCESS:
        # Retrieve image and 3D point cloud
        zed.retrieve_image(image_zed, sl.VIEW.LEFT)
        zed.retrieve_measure(point_cloud, sl.MEASURE.XYZ)

        # Convert to OpenCV format
        frame_rgba = image_zed.get_data()  # BGRA
        frame = cv2.cvtColor(frame_rgba, cv2.COLOR_RGB2BGR)  # Convert to BGR

        # MediaPipe detection
        results = hands.process(frame)

        if results.multi_hand_landmarks:
            for hand_landmarks in results.multi_hand_landmarks:
                mp_drawing.draw_landmarks(
                    frame, hand_landmarks, mp_hands.HAND_CONNECTIONS)

                palm = hand_landmarks.landmark[0]  # WRIST
                h, w, _ = frame.shape
                cx, cy = int(palm.x * w), int(palm.y * h)

                # Retrieve 3D coordinates from ZED
                point3d = point_cloud.get_value(cx, cy)[1]
                x_val, y_val, z_val = point3d[0], point3d[1], point3d[2]

                print(
                    f"Palm 3D Position: X={x_val:.3f}, Y={y_val:.3f}, Z={z_val:.3f} m"
                )

                # Optionally draw a circle on the palm
                cv2.circle(frame, (cx, cy), 5, (0, 255, 0), -1)

        cv2.imshow("ZED + MediaPipe Hand Tracking", frame)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

cv2.destroyAllWindows()
hands.close()
zed.close()