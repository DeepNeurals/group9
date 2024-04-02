import numpy as np
import cv2
import os

if __name__ == "__main__":
    # Input and output directories
    input_dir = "./images_orange"
    output_dir = "./output_FAST_orange/"

    # Create the output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)

    # Get a list of files in the input directory
    filenames = [filename for filename in os.listdir(input_dir) if filename.endswith('.jpg')]
    filenames.sort()

    # Loop over consecutive pairs of images
    for i in range(len(filenames) - 1):
        # Load two consecutive frames
        frame1 = cv2.imread(os.path.join(input_dir, filenames[i]))
        
        # Define the region of interest (ROI) as the left half of the image
        height, width, _ = frame1.shape
        roi = frame1[:, width//3:]

        # Convert the ROI to grayscale
        gray = cv2.cvtColor(roi, cv2.COLOR_BGR2GRAY)

        # Create a FAST object
        fast = cv2.FastFeatureDetector_create()

        # Detect corners using FAST
        keypoints = fast.detect(gray, None)

        # Draw detected corners on the ROI
        roi_with_corners = cv2.drawKeypoints(roi, keypoints, None, color=(255, 0, 0))

        # Copy the ROI with detected corners back to the original image
        image_with_corners = np.copy(frame1)
        image_with_corners[:, width//3:] = roi_with_corners

        # Save the result
        output_filename = os.path.join(output_dir, f"result_{i}.jpg")
        cv2.imwrite(output_filename, image_with_corners)
        print("Processed", output_filename)
