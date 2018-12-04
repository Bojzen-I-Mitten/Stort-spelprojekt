import glob
import sys
import os

def convertFile(filename):
        mipmaplvl = 0
        format = "DXT1"
        fl = 11.1
        if_filter = "CUBIC_DITHER"
        output = ''.join([x + "\\" for x in filename.split('\\')][:-1])
        print(output)
        temp_string = filename.split("_");
        try:
            parsed_string = temp_string[1].split(".")[0]

            if parsed_string.lower() == "normal":
                if_filter = "CUBIC"
                retVal = os.system("texconv.exe -if {} -f {} -m {} -fl {} -o {} -r {}".format(if_filter, format, mipmaplvl, fl, output, filename))

                if (retVal is not 0):
                    print("Failed to create new texture")

            elif parsed_string.lower() == "color":
                retVal = os.system("texconv.exe -if {} -f {} -m {} -fl {} -o {} -r {}".format(if_filter, format, mipmaplvl, fl, output, filename))
                if (retVal is not 0):
                    print("Failed to create new texture")

            elif parsed_string.lower() == "grey":
                format = "A8_UNORM"
                retVal = os.system("texconv.exe -if {} -f {} -m {} -fl {} -o {} -r {}".format(if_filter, format, mipmaplvl, fl, output, filename))

                if (retVal is not 0):
                    print("Failed to create new texture")

            else:
                print("I have no idea what {}".format(filename))

        except IndexError:
            print("This failed {}".format(filename))

def convertFileExtension(directory):
    for filename in glob.iglob(directory, recursive=True):
        convertFile(filename)

if __name__ == "__main__":
    for filename in glob.iglob('Textures/**/*.png', recursive=True):
        convertFileExtension(filename)
    for filename in glob.iglob('Textures/**/*.jpg', recursive=True):
        convertFileExtension(filename)
