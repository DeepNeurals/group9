import numpy as np
import cv2
import os

if __name__ == "__main__":
    # Input and output directories
    input_dir = "./images_pole"
    output_dir = "./theo2/"

    # Create the output directory if it doesn't exist
    os.makedirs(output_dir, exist_ok=True)

    # Get a list of files in the input directory
    filenames = [filename for filename in os.listdir(input_dir) if filename.endswith('.jpg')]
    filenames.sort()


    # Loop over consecutive pairs of images
    for i in range(len(filenames) - 1):
        # Load two consecutive frames
        frame1 = cv2.imread(os.path.join(input_dir, filenames[i]))
        
        # Convert frames to grayscale
        grey1 = cv2.cvtColor(frame1, cv2.COLOR_BGR2GRAY)

        # Perform edge detection (e.g., using Canny edge detector)
        edges1 = cv2.Canny(grey1, 50, 150)

        # Save the result
        output_filename = os.path.join(output_dir, f"result_{i}.jpg")
        cv2.imwrite(output_filename, edges1)
        print("Processed", output_filename)
