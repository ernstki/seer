
# Seer Change Log

## [1.12beta] - 2022-MM-DD

* Added Gdb's Non-Stop mode.
    - https://sourceware.org/gdb/onlinedocs/gdb/Non_002dStop-Mode.html
    - A new --non-stop-mode <yes|no>  command line flag
    - A new entry in the Debug dialog.
    - A new entry in Seer's config dialog under the gdb section.
* Added support for the older QT5.12 library.
    - Expect rendering problems with the builtin help system.
    - Urge to move to QT5.15 library.
* Main execute buttons.
    - Add '--all threads' for main execute buttons.
        - run/start/next/step/finish/continue/interrupt.
    - Individual threads can be managed by the Thread Manager.
* Thread Manager changes.
    - Thread Frame browser.
        - Allow multiple threads to be select. Selecting 1 thread will select stack frame.
        - Add next/step/finish/continue/interrupt for selected thread(s).
    - Thread Id browser.
        - Allow multiple threads to be select. Selecting 1 thread will select stack frame.
        - Add next/step/finish/continue/interrupt for selected thread(s).
    - Thread Group browser.
        - Allow multiple thread groups to be select.
        - Add continue/interrupt for selected thread group(s).
        - Add run/start for selected thread group(s). Disabled for now.
* Changing the Qt Style can be saved using Settings->Save Configuration.

## [1.11] - 2022-09-26

* Thread/Process manager.
    - Add a fork/vfork mode. Follow:
        - Parent process.
        - Child process.
        - Both (switchable via thread manager)
        - (see gdb's follow-fork-mode and detach-on-fork modes)
    - Add thread execution mode. Execute:
        - Selected thread only.
        - All threads
        - (see gdb's scheduler-locking mode)
    - Add multi-process execution mode.
        - Execute selected process.
        - Execute all processes.
        - (see gdb's schedule-multiple mode)
* Add Help to Thread/Process browser.
* Add Help to Stack Frame browser.
* Add Help to Variable/Register browser.
* Add Help to Source/Symbol/Library browser.
* Add Help to Struct Visualizer.
* Add Help to Memory Visualizer.
* Add Help to Array Visualizer.
* Add Help to Code Manager.
* Add Help to Breakpoint/Gdb log Manager.
* Add RMB menu to progress indicator to select indicator type.

## [1.10] - 2022-09-22

* New Struct Visualizer.
    - Built apon gdb's -var-obj framework.
    - Can recursively show the contents of a struct/class.
    - Show datatype of each member. (Value or pointer)
    - Can modify values of simple datatypes.
* Improve Source Browsing.
    - See new options in 'Source' config page.
    - Ignore opening files from a list of directories. Like system directories that don't exist.
    - Better sorting of program's files into "Source", "Header", and "Misc" folders in the
      SourceBrowser by having a list of filename patterns for each folder.
* The list of C/C++ file suffixes for syntax highlighting is configurable.
* Changed to CreativeCommons and GPLv3 icons.
    - Friendlier license to use icons.
    - Breeze Icon set for most things. (GPLv3)
    - Icons from these sites that require CreativeCommon license. (Simple attribution).
        icon-icons.com
        thenounproject.com

## [1.9] - 2022-08-30

* Rename 'seer' binary to 'seergdb'. The name conflicts with another opensource project.
* Prepare for better Debian (and other distros) packaging and releasing.
* Add history (up/down arrows) to all line input fields.
    - Variable logger, tracker browsers.
    - Source, function, types, statics, library browsers.
    - Struct, memory, array visualizers.
    - Code and assembly editors.
    - Console.
* Add StructVisualizer to source/assembly editors and stack arguments/locals browsers.
* Add a RMB to launch Memory/Array/Struct viewer from a running Struct viewer.
* Change 'detachtab' to use RMB context menu instead of a double-click.
* Allow filenames with spaces when setting breakpoints.

## [1.8] - 2022-08-08

* Add StructVisualizer
    - New visualizer to view the contents of a C/C++ struct or a C++ class in a tree.
    - Click on the "StructVisualizer" in the menubar or "View -> Struct Visualizer".
    - Enter the name of the variable in the text section.
    - examples:
        "*this"
        "mystruct"
        "mystruct.member"
    - The variable text section has a history. So you can use the UP and DOWN arrows
      to select previous entries.
    - Manually refresh the view or click on "Auto" to refresh the view at each
      stopping point (after a step/next/breakpoint).

* Add Dark theme option for Seer's code editor. Set in Seer config page.
    - Works for C/C++ source tabs and the assembly tab.

* Register browser.
    - Can now edit register values.
    - Either by double-clicking the register value or by RMB context menu
      to bring up a dialog.
    - Add format selector for register values.
    - Improve detection of changed register values from previous stopping point.

* Assembly support.
    - Add assembly config page for certain assembly settings.
        - assembly style
        - C++ symbol demangling.
        - Keep assembly tab on top.
        - Show assembly tab on Seer startup.
    - Launch MemoryVisualizer and ArrayVisualizer from the Assembly tab on highlighter
      text.

* Debug options for launching program.
    - No break on program start.
    - Break in "main".
    - Break in function.
    - Break at address.
    - Randomize program's start address.

* ArrayVisualizer and MemoryVisualizer now allow an address as the variable name.

* A menu to experiment setting Seer with different Qt gui styles.
    - See "View -> Styles"

* Bugs fixes.
    - Fixed double refresh bug in code editor.
    - Fixed bug when displaying assembly for the first time. It wasn't showing
      the current line.
    - Fixed background color problem in MemoryVisualizer for disassembly.

## [1.7] - 2022-07-04

* Add an assembly tab, along side the source tabs in the Code Manager.
    - Shows the program's assembly.
    - Enable with "View -> Assembly View"
    - Can set and show breakpoints.
    - Highlight the current instruction.
    - Step by instruction (Nexti and Stepi).
    - Double-clicking on entries in the "Breakpoints" tab and the "Stack frames" tab will show the assembly for those addresses.
    - ^F to bring up search bar in Assembly tab.
* Fix some minor bugs.

