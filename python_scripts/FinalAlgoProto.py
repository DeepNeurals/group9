import cv2
import os
import glob
import numpy as np

def crop_right(image):
    # Get image dimensions
    height, width = image.shape[:2]
    
    # Calculate the width to be cropped
    crop_width = int(width * 0.3)
    
    # Crop the image
    cropped_image = image[:, :-crop_width]
    
    return cropped_image


def change_black_to_white(img):
    # Convert image to YCrCb color space
    img_ycrcb = cv2.cvtColor(img, cv2.COLOR_BGR2YCrCb)
    img_copy = img_ycrcb.copy()

    # Define minimum and maximum threshold values for black or dark gray pixels
    y_min_threshold_black = 70  # Adjust this threshold value as needed
    y_max_threshold_black = 75  # Adjust this threshold value as needed
    cr_min_threshold_black = 80  # Adjust this threshold value as needed
    cr_max_threshold_black = 135  # Adjust this threshold value as needed
    cb_min_threshold_black = 100  # Adjust this threshold value as needed
    cb_max_threshold_black = 135  # Adjust this threshold value as needed

    # Define minimum and maximum threshold values for blue pixels
    y_min_threshold_blue = 65  # Adjust this threshold value as needed
    y_max_threshold_blue = 180  # Adjust this threshold value as needed
    cb_min_threshold_blue = 100  # Adjust this threshold value as needed
    cb_max_threshold_blue = 170  # Adjust this threshold value as needed
    cr_min_threshold_blue = 80  # Adjust this threshold value as needed
    cr_max_threshold_blue = 123  # Adjust this threshold value as needed

    # Iterate through all pixels in the image
    for i in range(img_ycrcb.shape[0]):
        for j in range(img_ycrcb.shape[1]):
            # Check if the pixel is black or dark gray
            if (y_min_threshold_black <= img_ycrcb[i, j, 0] <= y_max_threshold_black) and \
               (cr_min_threshold_black <= img_ycrcb[i, j, 1] <= cr_max_threshold_black) and \
               (cb_min_threshold_black <= img_ycrcb[i, j, 2] <= cb_max_threshold_black):
                
                img_copy[i, :, :] = [0, 0, 0] # Color the pixel in black

            elif (y_min_threshold_blue <= img_ycrcb[i, j, 0] <= y_max_threshold_blue) and \
                 (cr_min_threshold_blue <= img_ycrcb[i, j, 1] <= cr_max_threshold_blue) and \
                 (cb_min_threshold_blue <= img_ycrcb[i, j, 2] <= cb_max_threshold_blue):
                img_copy[i, :, :] = [0, 0, 0] # Color the pixel in black

    for i in range(img_ycrcb.shape[0]):
        for j in range(img_ycrcb.shape[1]):
            # Check if the pixel is black or dark gray
            if (y_min_threshold_black <= img_ycrcb[i, j, 0] <= y_max_threshold_black) and \
               (cr_min_threshold_black <= img_ycrcb[i, j, 1] <= cr_max_threshold_black) and \
               (cb_min_threshold_black <= img_ycrcb[i, j, 2] <= cb_max_threshold_black):
                
                img_copy[i, j, :] = [255, 255, 255] # Color the pixel in black

            elif (y_min_threshold_blue <= img_ycrcb[i, j, 0] <= y_max_threshold_blue) and \
                 (cr_min_threshold_blue <= img_ycrcb[i, j, 1] <= cr_max_threshold_blue) and \
                 (cb_min_threshold_blue <= img_ycrcb[i, j, 2] <= cb_max_threshold_blue):
                img_copy[i, j, :] = [255, 0, 255] # Color the pixel in black
    return img_copy


folder_path = './screenTrain/' # containing the images
image_files = sorted(glob.glob(folder_path + '*.jpg'))

i = 0
image_path = image_files[i]
original_img = cv2.imread(image_path)
cropped_img = crop_right(original_img)
cv2.imshow("Cropped Image", cropped_img)


modified_image = change_black_to_white(cropped_img)
cv2.imshow("Modified Image", modified_image)

cv2.waitKey(0)
cv2.destroyAllWindows()
