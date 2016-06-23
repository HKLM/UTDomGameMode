# UTDomGameMode
Domination Game Modes C++ plug-in for Unreal Tournament 4

Public access to the source code for only the UTDomGameMode plug-in.

Author: Brian 'Snake' Alexander  sw_snakepliskin@hotmail.com  
Date: 6/22/2016


### REQUIREMENTS:
UT4 Version: `4.12.0-2984440+++depot+UE4-UT-Releases`


## COMPILE THE PLUGIN
You must have Visual Studio 2015 and a current copy of the UnrealTournament (release branch) source code from GitHub.

1. Place the source files/Clone the UTDomGameMode GitHub respository to the plugins to your `<UnrealTournament GitHub project root>\UnrealTournament\Plugins` folder
  * VERIFY: UTDomGameMode.uplugin is at `<UnrealTournament GitHub project root>\UnrealTournament\Plugins\UTDomGameMode\UTDomGameMode.uplugin` and the Source folder is at `<UnrealTournament GitHub project root>\UnrealTournament\Plugins\UTDomGameMode\Source\`
2. Run the GenerateProjectFiles.bat file.
3. Open the UE4.sln
4. Set the build configuration to "Development Editor" and build the UnrealTournament project.
  * Once the build has finished follow the instruction on how to Install for Editor below.
5. Set the build configuration to "Shipping" and build the UnrealTournament project again.
  * Once the build has finished follow the instruction on how to Install for Game below.


## TO INSTALL FOR GAME
Requires the plugin to have been built using the "Shipping" configuration ( `UE4-UTDomGameMode-Win64-Shipping.dll` and `UE4-Win64-Shipping.modules` files) 

Follow the instructions for Distributing a Plugin starting at step 6 https://docs.unrealengine.com/latest/INT/Programming/Plugins/index.html#distributingaplugin
The staging folder would be your `UnrealTournamentDev\UnrealTournament\Plugins` folder of the launcher game. (see Example 1 below)

###### VERIFY
If installed correctly you should have a folder named "UTDomGameMode" in your plugins folder. The install path shown here is the default location. If you installed UnrealTournament game to another location, use your actual location. (See Example 2 below for example location of all files)

##### Example 1
```
C:\Program Files\Epic Games\UnrealTournamentDev\UnrealTournament\Plugins\
```
##### End Example 1


#####  Example 2
```
C:\Program Files\Epic Games\UnrealTournamentDev\UnrealTournament\Plugins\UTDomGameMode\UTDomGameMode.uplugin

C:\Program Files\Epic Games\UnrealTournamentDev\UnrealTournament\Plugins\UTDomGameMode\Binaries\Win64\UE4-UTDomGameMode-Win64-Shipping.dll
C:\Program Files\Epic Games\UnrealTournamentDev\UnrealTournament\Plugins\UTDomGameMode\Binaries\Win64\UE4-Win64-Shipping.modules

C:\Program Files\Epic Games\UnrealTournamentDev\UnrealTournament\Plugins\UTDomGameMode\Config\DefaultGame.ini
```
#####  End Example 2


##### NOTE
You must have at least one DOM map installed for the mod to work and show up with in the game.


## EDITOR SETUP
Requires the plugin to have been built using the "Development Editor" and "Shipping" configurations ( `UE4-UTDomGameMode-Win64-Shipping.dll` `UE4-Win64-Shipping.modules` `UE4Editor-UTDomGameMode.dll` and `UE4Editor.modules` files). 

Follow the instructions for Distributing a Plugin starting at step 6 https://docs.unrealengine.com/latest/INT/Programming/Plugins/index.html#distributingaplugin  The staging folder would be your `UnrealTournamentEditor\UnrealTournament\Plugins` folder of the launcher editor. (see Example 3 below)

###### VERIFY
If installed correctly you should have a folder named "UTDomGameMode" in your plugins folder. (See Example 4 below for example location of all files)  The install path shown here is the default location. If you installed UnrealTournament Editor to another location, use your actual location.


##### Example 3
```
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\
```
##### End Example 3



##### Example 4
```
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\UTDomGameMode.uplugin

C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Binaries\Win64\UE4-UTDomGameMode-Win64-Shipping.dll
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Binaries\Win64\UE4-Win64-Shipping.modules
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Binaries\Win64\UE4Editor-UTDomGameMode.dll
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Binaries\Win64\UE4Editor.modules

C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Config\DefaultGame.ini

C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\DomB.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\DomGN.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\DomGold.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\DomNULL.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\DomR.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\DomX.uasset

C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Sounds\ControlSound.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Sounds\ControlSound_Cue.uasset

C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\BaseSkin_Mat.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\Blank_Mat.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\BlueTeamSymbol.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DomHUDAtlas01.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\goldskin2.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\GoldTeamSymbol.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\GraySkin.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\GreenTeamSymbol.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\NeutralSymbol.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\RedTeamSymbol.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\Skin0_Mat_Inst.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\Skin1_Mat_Inst.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\Skin2_Mat.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\Skin3_Mat.uasset
C:\Program Files\Epic Games\UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\Skin4_Mat_Inst.uasset
```
##### End Example 4


## HOW TO COOK/SHARE DOM MAP
1. You must save your map with the prefix **DOM-** in the file name (example: DOM-myMapName)
2. From within the Editor, Go to Settings\-\>World Settings
3. Set GameMode Override to **Domination**
4. Edit the Project Settings (Toolbar\-\>Edit\-\>Project Settings)
5. Select the Project - Packaging section
6. Add a **new element** to the **Additional Asset Directories to Cook** list and manually type in the following path 
	```
	UTDomGameMode/UTDomGameContent
	```
7. Add a **new element** to the **Additional Non-Asset Directories to Package** list and manually type in the following path 
	```
	UTDomGameMode/UTDomGameContent
	```
8. Toolbar\-\>Share\-\>Share This Level



##### NOTE ON COOKING
The cooker seems to want the map and all content assets used in the map, to be in or in a subfolder of the  
`<Editor install location>\UnrealTournament\Content\RestrictedAssets` folder. The cooker will not complete if any of the maps assets, or the map itself is not in this area.


## KNOWN ISSUES
| item | description |
| --- | --- |
| Issue | Error Message "Plugin \'UTDomGameMode\' failed to load because module \'UTDomGameMode\' does not appear to be compatible with the current version of the engine. The plugin may need to be recompiled." |
| Fix | After a update to UT4 has been done, you must Uninstall this plugin from your UT4 launcher game (or only delete the UTDomGameMode.uplugin file) before starting UT4. Otherwise you will receive that error message. |
