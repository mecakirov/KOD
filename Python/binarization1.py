import cv2
import numpy as np
import skimage.filters as filters

# read the image
img = cv2.imread(r"C:\Users\muham\Documents\GitHub\KOD\Python//image7.png")

# convert to gray
gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

# blur
smooth = cv2.GaussianBlur(gray, (95,95), 0)

# divide gray by morphology image
division = cv2.divide(gray, smooth, scale=255)


# sharpen using unsharp masking

sharp = filters.unsharp_mask(division, radius=0.5, amount=1.5,  preserve_range=False,channel_axis=False)

sharp = (255*sharp).clip(0,255).astype(np.uint8)



# threshold
thresh = cv2.threshold(sharp, 0, 255, cv2.THRESH_BINARY + cv2.THRESH_OTSU)[1]

# save results
cv2.imwrite('receipt_division.png',division)
cv2.imwrite('receipt_division_sharp.png',sharp)
cv2.imwrite('receipt_division_thresh.png',thresh)


# show results
cv2.imshow('smooth', smooth)  
cv2.imshow('division', division)  
cv2.imshow('sharp', sharp)  
cv2.imshow('thresh', thresh)  
cv2.waitKey(0)
cv2.destroyAllWindows()