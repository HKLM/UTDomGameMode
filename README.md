# UTDomGameMode
Domination Game Modes C++ plug-in for Unreal Tournament 4

Public access to the source code for the UTDomGameMode plug-in.

## ALL DOM GAME MODES HAVE UP TO 4 TEAM PLAY
## ADDED DOUBLE DOMINATION GAME MODE

Version: Beta 3.4.2
Author: Brian 'Snake' Alexander  sw_snakepliskin@hotmail.com
Date: 3/15/2017


### REQUIREMENTS:
UT4 Version: `build 3315666`


##### NOTES
- The folder paths shown in this readme file are the default location UT4 is installed at. If you have installed the game and/or editor to another location, make the appropriate changes to the file paths.
- The released binary files have been compiled against the current "Release" GitHub branch of UT4 that is compatible with the Epic Game's Launcher version of UT4. These files will not work with any other GitHub branch and/or version. This is most likely is **NOT** compatible with the "Clean-Master" GitHub UT4 branch at all!
- You must have **at least one DOM map installed** for the mod to work and **show up with in the game**.
- The file "UTDomGameVersion.dll" is of no use to the game or editor. This is only a file used by the Microsoft Windows Installer (MSI) to perform file versioning on unversioned files of this plugin.



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
The staging folder would be your `UnrealTournament\UnrealTournament\Plugins` folder of the launcher game. (see **"LAUNCHER GAME" - DEFAULT FILE INSTALL LOCATIONS** section below)


## EDITOR SETUP
Requires the plugin to have been built using the "Development Editor" and "Shipping" configurations ( `UE4-UTDomGameMode-Win64-Shipping.dll` `UE4-Win64-Shipping.modules` `UE4Editor-UTDomGameMode.dll` and `UE4Editor.modules` files). 

Follow the instructions for Distributing a Plugin starting at step 6 https://docs.unrealengine.com/latest/INT/Programming/Plugins/index.html#distributingaplugin  The staging folder would be your `UnrealTournamentEditor\UnrealTournament\Plugins` folder of the launcher editor. (see **"LAUNCHER" EDITOR - DEFAULT FILE INSTALL LOCATIONS** section below)



## QUICK START ON DOMINATION MAP MAKING:
1. You must save your map with the prefix **DOM-** in the file name (example: DOM-myMapName)
2. From within the Editor, Go to Settings\-\>World Settings
3. Set GameMode Override to **Domination**
4. Place at least 1 (3 is recommended) ControlPoint actors in to your map.
5. On each ControlPoint actors' details. Set the "Point Name" to the name/title for this point.


## HOW TO COOK/SHARE DOMINATION MAP:
1. Toolbar\-\>Build
2. Toolbar\-\>Share\-\>Share This Level



##### NOTE ON COOKING
The cooker seems to want the map and all content assets used in the map, to be in or in a subfolder of the  
`<Editor install location>\UnrealTournament\Content\RestrictedAssets` folder. The cooker will not complete if any of the maps assets, or the map itself is not in this area.



## KNOWN ISSUES
| item | description |
| --- | --- |
| Issue | Error Message "Plugin \'UTDomGameMode\' failed to load because module \'UTDomGameMode\' does not appear to be compatible with the current version of the engine. The plugin may need to be recompiled." |
| Fix | After a update to UT4 has been done, you must Uninstall this plugin from your UT4 launcher game (or only delete the UTDomGameMode.uplugin file) before starting UT4. Otherwise you will receive that error message. |
| Issue | Players who do not have this plugin installed, when joining an online Domination game. The map maybe auto-downloaded, but will fail to load or the game may crash. |
| Fix | Players must have this plugin installed, prior to joining an online Domination game. |




#####  UT4 "LAUNCHER GAME" - DEFAULT FILE INSTALL LOCATIONS
```
C:\Program Files\Epic Games\UnrealTournament\UnrealTournament\Plugins\UTDomGameMode\UTDomGameMode.uplugin
C:\Program Files\Epic Games\UnrealTournament\UnrealTournament\Plugins\UTDomGameMode\UTDomGameVersion.dll
C:\Program Files\Epic Games\UnrealTournament\UnrealTournament\Plugins\UTDomGameMode\UTDomGame_README.txt

C:\Program Files\Epic Games\UnrealTournament\UnrealTournament\Plugins\UTDomGameMode\Binaries\Win64\UE4-UTDomGameMode-Win64-Shipping.dll
C:\Program Files\Epic Games\UnrealTournament\UnrealTournament\Plugins\UTDomGameMode\Binaries\Win64\UE4-Win64-Shipping.modules

C:\Program Files\Epic Games\UnrealTournament\UnrealTournament\Plugins\UTDomGameMode\Config\DefaultGame.ini
```


##### UT4 "LAUNCHER" EDITOR - DEFAULT FILE INSTALL LOCATIONS:
```
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\UTDomGameMode.uplugin

UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Binaries\Win64\UE4Editor-UTDomGameMode.dll
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Binaries\Win64\UE4Editor.modules

UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Config\DefaultGame.ini

UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\BaseUTDomCharacter.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\DefaultDomCharacter.uasset

UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\DomB.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\DomGN.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\DomGold.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\DomR.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\DomX.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\xDomA.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\xDomB.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\xDomBase.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Meshes\xDomRing.uasset

UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Sounds\ControlSound.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Sounds\ControlSound_Cue.uasset

UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\BaseSkin_Mat.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\BlueTeamSymbol.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DomHUDAtlas01.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DOM_HUDAtlas01.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\goldskin2.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\GoldTeamSymbol.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\GraySkin.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\GreenTeamSymbol.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\NeutralSymbol.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\RedTeamSymbol.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\Skin0_Mat_Inst.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\Skin1_Mat_Inst.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\Skin2_Mat.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\Skin3_Mat.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\Skin4_Mat_Inst.uasset

UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DDomA.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DDomB.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DominationPointTex.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMLetter_0.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMLetter_1.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMLetter_2.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMLetter_3.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMLetter_4.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMLetter_MASTER.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMPointABb.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMPointABg.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMPointABgold.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMPointABgreen.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMPointABr.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMpointBLUE.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMpointGOLD.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMpointGREEN.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMpointGREY.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMpointLINESb.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMpointLINESg.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMpointLINESgold.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMpointLINESgreen.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMpointLINESr.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMpointRED.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DomPoint_Mat_MASTER.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DomPoint_MI_0.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DomPoint_MI_1.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DomPoint_MI_2.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DomPoint_MI_3.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DomPoint_MI_4.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DomPoint_MI_5.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMRing_0.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMRing_1.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMRing_2.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMRing_3.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\DOMRing_4.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\PickupLines2.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\Static.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\xDomBase_N2.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\xDomBase__Height_.uasset
UnrealTournamentEditor\UnrealTournament\Plugins\UTDomGameMode\Content\UTDomGameContent\Textures\DDom\xDomBase__Specular_.uasset

```
