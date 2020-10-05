# Threshold
A simple Unreal Engine action game!

## What is it?
This is a simple Unreal Engine action game! (It's right there, duh :P)

More seriously, this is a project I've been working on for a few months now, to teach myself more Unreal Engine programming. My goal is to create as much of this game (animations, gameplay systems, effects, etc.) from scratch so I can get a better understanding of all of the systems required for making a project in the Unreal Engine.

There will most likely reach a point where in order to have a nice polished appearance and be something I'm willing to share outside of short demo videos, this project will need assets not created by me, but it is currently far from that point.

The main goal is to have a combat system akin to that seen in many action RPGs such as the Souls series and Legend of Zelda. Another main goal is to ensure that all of the code and systems are set up cleanly and consistently so that the code is approachable to other people (and myself!) and easily extensible, taking full advantage of the Unreal Engine gameplay framework.

As of now, all code and assets are released under the MIT license with the intention of hopefully allowing other developers to learn from my work. I don't find it likely that anyone will use code or assets from this project directly, but if you do, please credit me visibly somewhere!

## Features

### Networking

A major goal for this project is to support networked multiplayer. Unreal's out of the box character actor supports client-side predicted movement fairly reliably.
Combat is implemented using the Engine's experimental Gameplay Ability System Plugin.

(While the plugin itself is still in Beta, it is used extensively in Fortnite and the now-cancelled Paragon, so it should be reasonably stable).

Inventory is handled using a semi-custom solution built on top of the standard fast array replication system, with a lot of custom serialization code to support polymorphic data.

### Inventory

The inventory system, as mentioned in networking features, contains custom serialization code in order to be more extensible and support polymorphic data.
Below is a list of the main components in the inventory system.

#### Inventory Item

This is the key component in the inventory system, a struct that holds an inventory item type asset and inventory item data. Its main purpose is to wrap the type asset and item data for network replication and save serialization.
The inventory item also holds a few convenience functions for interfacing with the underlying data. It is mainly intended to be stored in an inventory component, but it can be used in other property slots.

#### Inventory Item Type

Inventory item types are UObjects used for comparing inventory items by type, and storing data about the inventory items that is designed to not change (or change very infrequently) at runtime.
They also describe the functionality of the inventory item and act as the interface between the inventory item and the item data.

All inventory item operations (stack count access, item description queries, etc.) are funnelled through the item type and provide the optional item data.
In addition to being required to interact with the item data (the inventory item structs have no logical knowledge about the underlying item data besides static reflection data required for serialization), this allows the item type to change the description and other information about the item based on the optional data.

Item types, as with most UObjects, can be replicated easily across the network via object reference if they are loaded from a package and not modified (e.x. as an instanced property in a level that hasn't changed at runtime); however,
by implementing a net serialization function, the inventory item struct can replicate item types with runtime data changes.

When the item type is replicated by the item struct, the struct checks if the type object is supported for networking (i.e. stably named by virtue of being loaded directly from a package).
If the type object is, it is simply replicated via reference. If the type object is not supported for networking (e.x. it has been changed or created at runtime), then its class and custom serialization data are transmitted instead.
The client's receiving inventory item struct will create a new item type object from that class and load the serialized data.

Currently there are two example item types: items loaded from a datatable with and without stacking functionality.

#### Inventory Item Data

A simple POD struct to hold data relevant to any inventory type. The only required override exposes the static struct reflection data.

#### Inventory Component

Component that stores inventory items in an array, with stacking support. Stores all inventory items in a fast serializable array and is set to replicate by default.

## Coding Standards

Most of the [coding standards](https://docs.unrealengine.com/en-US/Programming/Development/CodingStandard/index.html) used for the Unreal Engine also apply for this project. I myself have been a bit lax with `const`-correctness for function arguments, but I will try to keep that in mind going forward. Additionally, I haven't been using JavaDoc style comments, as this project currently doesn't have any automated documentation, but that may change in the future.

That being said, I do have a few standards I would like to reiterate as extra important, and some additional standards not specified in the Unreal Engine coding standards:

### Ordering and Spacing

* Order of function definitions in `.cpp` files should match the order of definitions in the `.h` files
    * Though not strictly necessary given a good IDE and navigation shortcuts, it does make it easier to find the definitions for specific functions.
* Space out functions and variables and try to group them logically
    * In both headers and source files, try to group functions together by categories such as: engine overrides, accessors (getters/setters), helper functions, original (non-override) methods, etc. (See THCharacter for a good example)
    * Put a consistent number of spaces between functions and groups
        * My general rule of thumb is: 3 spaces between function/variable groups in a header, 4 spaces in a source file
        * One space between functions in source files
        * Use space between functions and variables in header files if you want to use comment tooltips - the Unreal Header Tool will generate a tooltip for `UPROPERTY()`s based on all comments before a variable without an empty newline, and most IDEs will provide similar tooltips on hovering over a function.
* At the top of a header or source file I use the following guidelines:
    * First line should be the copyright notice (right now everything is copyright me)
    * A single space
    * `#pragma once`
    * Another single space
    * All includes, starting with `CoreMinimal.h` and ending with `MyHeader.generated.h` (if needed)
    * Three more spaces
    * All the code!
    * A single newline at the end of the file

### What to Include

* Every `UCLASS`, `USTRUCT`, or `UINTERFACE` requires the inclusion of the `MyHeader.generated.h` file along with the appropriate `GENERATED_BODY()` in the class definition
* Anything that needs `UObject` or derived types should include `CoreMinimal.h`
* Only include other files in a header if you have to - forward declare everything else.
    * As a general rule, if the object is not a pointer or reference and is stored as a member variable, you need to include the appropriate header (the compiler needs to have the full definition of the object to determine the storage size of the class). Otherwise you can forward declare the type!
    * Alternatively, if you _really_ aren't sure, forward declare it anyway and see if your compiler or IDE gets mad at you
    * You can forward declare classes using an [elaborated type specifier](https://en.cppreference.com/w/cpp/language/elaborated_type_specifier) such as `void MyFunction(class USomeUnrealClass& ObjectArgument)` or `class USomeUnrealClass* MemberVariable = nullptr;` - this isn't generally done in C++ as far as I know, but I adopted it from some of the engine code since it makes it easier when you have a ton of different forward declarations required
* Include headers for all classes you need in your source files

### Naming
* Try to balance both consice and descriptive names
    * For instance `SearchForTheNearestActorInTheWorldByCollisionChannel(ECollisionChannel Channel)` would probably still make sense as `FindNearestActorByCollision(ECollisionChannel Channel)`, but use your best judgement
    * Likewise `NearestActor(ECollisionChannel E)` might be a bit too concise


## Contributing
While I am the only developer currently, so I haven't been following these guidelines, if you do end up helping in the future, try to follow these guidelines for git usage

* For branch and issue naming try to keep names short and use all lowercase letters with words separated by dashes
* Create an issue for each new feature, and write any useful information you come across about potential implementations, difficulties, and future improvements
* Create an entry in the GitHub Project board, and move it to the correct category as you progress
* Create a pull request when you are done
* For now, branches can be merged directly into `master` as there is no separation between a release or development branch currently. Everything is considered to be heavily in development!

## Planned Features
This might be changed in the future, and better organized using Wiki pages, but as of now all planned features are tracked in the Project board

