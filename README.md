# Daz To O3DE
* Owner: [Daz 3D][OwnerURL] — [@Daz3d][TwitterURL]
* License: [Apache License, Version 2.0][LicenseURL] - see ``LICENSE`` and ``NOTICE`` for more information.
* Official Project: [github.com/daz3d/DazToO3DE][RepositoryURL]

## Prerequisites
* A compatible version of the [Daz Studio][DazStudioURL] application
  * Minimum: 4.10
* A compatible version of the [O3DE][O3deURL] application
  * Minimum: 21.11
* Operating System
  * Windows 7 or newer

## Manual Installation
* Find the Newest Release [here][ReleasesURL].
 ### Steps to download a new release from GitHub
 * Go to the link listed above to find the newest release on GitHub.
 * Under 'Assets' you will find the newest build. It will be named 'DazToO3DE_VersionNumber'.
 * Download the zip and extract the contents of the bridge.
 * The extracted contents include two different directories, 'O3DE' and 'Daz Studio'.
 * Follow the installation steps as described in the sections below.

### Steps to install the bridge on the Daz Studio side
* The bridge can be manually installed to the Daz Studio with the help of the 'RunOnce' script.
* The manual process involves two different steps to complete the installation successfully.
* The first step is to copy the actual bridge scripts into the appropriate path.
  * Inside the extracted folder of the bridge, go under 'Daz Studio' and copy the 'o3deBridge' directory.
  * Now, go to the install location of Daz Studio. By Default that location will be ``C:\Program Files\DAZ 3D\DAZStudio4``
  * Navigate to the scripts location which is ``\scripts\support\DAZ``
  * Paste the copied 'o3deBridge' directory to the scripts location mentioned above.
* The second step is to use the 'RunOnce' script and install the bridge to the Daz Studio.
  * Inside the extracted folder of the bridge, go under 'Daz Studio' and copy the 'Daz to O3DE_Install.dsa' script.
  * Go to your 'AppData' Folder. Easiest Method is to press 'start', type ``%appdata%``, and enter.
  * Navigate to the 'RunOnce' location which is located at ``\AppData\Roaming\DAZ 3D\Studio4\RunOnce``
  * Paste the copied 'Daz to O3DE_Install.dsa' script to the 'RunOnce' location.
* Run the Daz Studio to finish the installation.
* Inside Daz Studio, the O3DE exporter script can be found under the 'Scripts' menu and in the 'Bridges' section, named as 'Daz to O3DE'.
* While updating to a newer version of the bridge, it is enough to replace the 'o3deBridge' directory as described in the first step of the manual installation.

### Steps to install the bridge on the O3DE side
* The bridge will be installed to O3DE as a gem.
* Inside the extracted folder of the bridge, go under '03DE' and copy the 'DazToO3DE' directory.
* Paste the copied 'DazToO3DE' directory to any location in the system where the O3DE gems are stored.
* Navigate to the path ``C:\Users\<username>\.o3de`` and open 'o3de_manifest.json' using a text editor.
* Read the 'o3de_manifest.json' file and find the ``external_subdirectories`` array type property.
* Note the path to the storage location of 'DazToO3DE' directory and add it to ``external_subdirectories`` array. This step will enable 'O3DE Project Manager' to look for the user installed gems in the system.
* Using 'O3DE Project Manager', create a new project and configure it with 'DazToO3DE' gem.
* Build and run the newly created project.
* 'Daz Importer' action can be found under 'Tools' menu which can be used to import an exported character asset from Daz Studio.
* While updating to a newer version of the bridge, replace the 'DazToO3DE' directory with the latest content.

## Directory Structure
Files in this repository are organized into two distinct top-level directories, named after the applications that the files within them relate to. Within these directories are hierarchies of subdirectories that correspond to locations on the target machine. Portions of the hierarchy are consistent between the supported platforms and should be replicated exactly while others serve as placeholders for locations that vary depending on the platform of the target machine.

Placeholder directory names used in this repository are:

Name  | Windows
------------- | -------------
appdata_common  | Equivalent to the expanded form of the `%AppData%` environment variable.  Sub-hierarchy is common between 32-bit and 64-bit architectures.
appdir_common  | The directory containing the primary executable (.exe) for the target application.  Sub-hierarchy is common between 32-bit and 64-bit architectures.

[OwnerURL]: https://www.daz3d.com
[TwitterURL]: https://twitter.com/Daz3d
[LicenseURL]: http://www.apache.org/licenses/LICENSE-2.0
[RepositoryURL]: https://github.com/daz3d/DazToO3DE
[DazStudioURL]: https://www.daz3d.com/get_studio
[O3deURL]: https://o3de.org/download
[ReleasesURL]: https://github.com/daz3d/DazToO3DE/releases
