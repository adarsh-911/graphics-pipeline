from PIL import Image

ppm_image = Image.open("output.ppm")
ppm_image.save("output.png", "PNG")

