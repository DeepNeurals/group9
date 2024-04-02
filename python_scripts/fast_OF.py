import cv2
import os
import numpy as np
from sklearn.cluster import DBSCAN
import numpy as np

def compute_depth(optical_flow):
    # Compute magnitude of optical flow vectors
    flow_magnitude = np.sqrt(optical_flow[..., 0]**2 + optical_flow[..., 1]**2)

    # Scale magnitude to range [0, 255]
    magnitude_scaled = cv2.normalize(flow_magnitude, None, 0, 255, cv2.NORM_MINMAX)

    # Invert magnitude to represent depth (farther objects have smaller magnitudes)
    depth = 255 - magnitude_scaled
    return depth.astype(np.uint8)

if __name__ == "__main__":
    # Input and output directories
    input_dir = "./images_pole/"
    output_dir = "./theo1/"

    # Create the output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)

    # Get a list of files in the input directory
    filenames = [filename for filename in os.listdir(input_dir) if filename.endswith('.jpg')]
    filenames.sort()

    # Create a FAST object
    fast = cv2.FastFeatureDetector_create()

    # Loop over consecutive pairs of images
    for i in range(len(filenames) - 1):
        # Load two consecutive frames
        frame1 = cv2.imread(os.path.join(input_dir, filenames[i]))
        frame2 = cv2.imread(os.path.join(input_dir, filenames[i + 1]))
        
        # Convert frame to grayscale
        grey1 = cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY)
        grey2 = cv2.cvtColor(frame2, cv2.COLOR_BGR2GRAY)

        # Detect keypoints using FAST
        keypoints1 = fast.detect(grey1, None)
        keypoints2 = fast.detect(grey2, None)

        # Create a blank image
        keypoints_img = np.zeros_like(grey1)
        keypoints_img2 = np.zeros_like(grey2)

        # Draw a pixel at the locations of keypoints
        for kp in keypoints1:
            x, y = map(int, kp.pt)
            keypoints_img[y, x] = 255
        
        for kp in keypoints2:
            x, y = map(int, kp.pt)
            keypoints_img2[y, x] = 255


        # Extract (x, y) coordinates from cv2.KeyPoint objects
        keypoint_coords = np.array([kp.pt for kp in keypoints1])

        # Apply DBSCAN to reduce the noise
        dbscan = DBSCAN(eps=10, min_samples=10).fit(keypoint_coords)

        # Get the indices of core points
        core_point_indices = np.where(dbscan.labels_ != -1)[0]

        # Extract core points
        core_points = keypoint_coords[core_point_indices]

        # # Create an image to visualize the core points
        core_points_img = np.zeros_like(frame1)
        # for point in core_points:
        #     x, y = map(int, point)
        #     core_points_img[y, x] = (0, 255, 0)

        # Plot core points on the image
        point_size = 2

        for point in core_points:
            x, y = map(int, point)
            cv2.circle(core_points_img, (x, y), point_size, (0, 255, 0), -1)

        # optical_flow = cv2.calcOpticalFlowFarneback(keypoints_img, keypoints_img2, None, 0.6, 4, 10, 5, 7, 1.2, 0)
        # depth_ratio1 = compute_depth(optical_flow)


         # Save the result
        output_filename = os.path.join(output_dir, f"result_{i}.jpg")
        cv2.imwrite(output_filename, core_points_img)
        print("Processed", output_filename)

        # # Save and display the core points image
        # cv2.imwrite("core_points_img.jpg", core_points_img)
        # cv2.imshow("Core Points Image", core_points_img)
        # cv2.waitKey(0)
        # cv2.destroyAllWindows()

        # # Save the result
        # output_filename = os.path.join(output_dir, f"keypoints_{i}.jpg")
        # cv2.imwrite(output_filename, keypoints_img)
        # print("Processed", output_filename)
