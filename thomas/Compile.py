"""
Builds thomas game using absolute path

"""
import sys
import os
from distutils.dir_util import copy_tree
from shutil import copyfile

output = ""

if (len(sys.argv) < 2):
    print("No destination folder set")
    output = "CompiledGame"
else:
    output = sys.argv[1]

print("Output folder set to {}".format(output))

try:
    os.mkdir(output)
except FileExistsError:
    print("Could not create folder as it already exists")


try:
    os.mkdir(output + "\Bin")
except FileExistsError:
    print("Could not create folder as it already exists")


try:
    os.mkdir(output + "\Data")
except FileExistsError:
    print("Could not create folder as it already exists")


try:
    os.mkdir(output + "\Data\Assets")
except FileExistsError:
    print("Could not create folder as it already exists")


print("Started compilation")
os.system("csc -out:{}\Bin\My.exe -optimize -platform:x64 -warn:0 -nologo -reference:ReleaseEditorBuild64\ThomasEngine.dll, -reference:System.dll  -lib:%windir%\Microsoft.NET\Framework64\\v4.0.30319\ -reference:%windir%\Microsoft.NET\Framework64\\v4.0.30319\WPF\WindowsBase.dll  Data\\assemblyFiles\\ExportProject.cs".format(output))
print("Compilation done")

print("Moving files")
fromDirectory = "..\\Concussion Ball\Obj\Release\Assembly.dll"
toDirectory = output + "/Bin/Assembly.dll"

copyfile(fromDirectory, toDirectory)

fromDirectory = "ReleaseEditorBuild64/Newtonsoft.Json.dll"
toDirectory = output + "/Bin/Newtonsoft.Json.dll"

copyfile(fromDirectory, toDirectory)

fromDirectory = "ReleaseEditorBuild64/ThomasNet.dll"
toDirectory = output + "/Bin/ThomasNet.dll"

copyfile(fromDirectory, toDirectory)

fromDirectory = "ReleaseEditorBuild64/ThomasMath.dll"
toDirectory = output + "/Bin/ThomasMath.dll"

copyfile(fromDirectory, toDirectory)

fromDirectory = "ReleaseEditorBuild64/ThomasEngine.dll"
toDirectory = output + "/Bin/ThomasEngine.dll"

copyfile(fromDirectory, toDirectory)


fromDirectory = "Data"
toDirectory = output + "\Data\\"

copy_tree(fromDirectory, toDirectory)

fromDirectory = "..\\Concussion Ball\Assets\\"
toDirectory = output + "\Data\Assets"

copy_tree(fromDirectory, toDirectory)

fromDirectory = "..\\Concussion Ball\\Concussion Ball.thomas"
toDirectory = output + "\Data\Project.thomas"

copyfile(fromDirectory, toDirectory)
print("Done")
