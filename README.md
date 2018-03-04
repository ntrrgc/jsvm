**Important:** This project is still under heavy development. Some features mentioned here are not yet implemented and the API may have breaking changes every so often. Still, I'm leaving this README here so that the purpose of the project can be understood.

<hr/>

JSVM is a library that exposes JavaScript to Java using the [Duktape engine](http://duktape.org/). Using JSVM you can evaluate JS snippets from Java and receive wrapped JS objects that you can use to call their methods, assign properties etc. all from Java.

You can also expose Java objects to JS, enabling your JS code to call specific Java functions. Both ways of interaction can be combined easily in your code, so for instance you can invoke a JS function passing a Java callback as an argument.

Using JSVM you can embed JavaScript code in your Android or Java application, which can be useful in a variety of scenarios, such as multi-platform development where most of the code is written in JS in order to be run both in Android and iOS, plugin systems, or reusing existing JavaScript libraries.

## Installation (Android)

You can install this library from [JitPack](https://jitpack.io/#ntrrgc/jsvm). Add this to your `build.gradle`:

```
repositories {
    maven { url 'https://jitpack.io' }
}

dependencies {
    implementation 'com.github.ntrrgc.jsvm:jsvm-lib:master-SNAPSHOT' // Java classes
    implementation 'com.github.ntrrgc.jsvm:jsvm-native:master-SNAPSHOT' // Native library (.so)
}
```

You can change `SNAPSHOT` for an specific commit hash to get a fixed version of the library.

The wiki includes [instructions to build the library](https://github.com/ntrrgc/jsvm/wiki/Building-JSVM) in your own computer for development or to use it in a non-Android Java environment.

## How does it look?

Simple usage evaluating expressions:

```java
JSVM jsvm = new JSVM();
int result = jsvm.evaluate("2 + 5").asInt();
```

You can also use `eval()` to load libraries. Variables are persisted between `eval()` calls.

```java
JSVM jsvm = new JSVM();
jsvm.evaluate(fileRead("pokedex.js"));

jsvm.evaluate("var pokedex = new Pokedex();"
	+ "var lapras = pokedex.pokemon('lapras')");

System.out.printf("Name: %s\n", jsvm.evaluate("lapras.name").asString());
System.out.printf("Weight: %d\n", jsvm.evaluate("lapras.weight").asDouble());
```

What if the pokemon name was specified by the user? **Concatenating user input into anything called eval is always a terrible idea!** Luckily JSVM does not force us to. In fact, JSVM allows us to manipulate every possible JS object from Java without it! No JS compilation needed, just Java methods!

```java
void printPokemon(String pokemon) {
	JSFunction Pokedex = jsvm.getGlobalScope().get("Pokedex").asFunction();
	JSObject pokedex = Pokedex.callNew().asObject();

	JSObject pokemon = pokedex
		.invokeMethod("pokemon", JSValue.aString(pokemon))
		.asObject();

	System.out.printf("Name: %s\n", pokemon.get("name").asString());
	System.out.printf("Weight: %f\n", pokemon.get("weight").asDouble());
}
```

## How is it different from Duktape or other Duktape bindings?

Duktape is an embeddable JS engine written in C with an emphasis on portability and memory footprint. As such, Duktape exposes a C API that C developers can use to embed the engine in their applications.

The Duktape API is pretty low level though. Using it requires you to think in terms of the execution stack. Most Duktape API functions map to instructions in the Duktape JS bytecode, so the code that uses Duktape API tends to look a lot like bytecode: long, hard to follow, hard to debug and tends to distract from the real problem at hand.

For instance, this is the code for the example at the top of this page, using *the abbreviated functions* of the Duktape API:

```c
duk_context* ctx = duk_create_heap_default();
// Evaluate the library
duk_eval_string_noresult(ctx, fileRead("pokedex.js"));

// Put the Pokedex constructor function on the top of the stack
duk_push_global_object(ctx);
duk_get_prop_string(ctx, -1, "Pokedex");
// Invoke with the new operator and put the created object on the top of the stack
duk_new(ctx, 0);

// Call pokedex.pokemon(pokemonName) and put the result (pokemon) on top of the stack
duk_push_string(ctx, "pokemon");
duk_push_string(ctx, pokemonName);
duk_call_prop(ctx, -3, 1);

duk_get_prop_string(ctx, -1, "name");
// the "name property" is now at the top of the stack, read it
printf("Name: %s\n", duk_to_string(ctx, -1));
// remove the name from the stack in order to access pokedex, which is just below
duk_pop(ctx);

duk_get_prop_string(ctx, -1, "weight");
printf("Weight: %lf\n", duk_to_number(ctx, -1));
// remove the weight from the stack
duk_pop(ctx);

// clean the stack by removing pokemon, pokedex and the Pokedex constructor function
duk_pop_3(ctx);
```

Duktape has not a bad API for a JS engine and it's quite powerful in fact... But it's just too low level to use directly in applications! The goal of JSVM is to wrap this complexity behind an application-friendly API, safe and easy to use.

# JSVM internals

[You can find more about how JSVM works in the wiki.](https://github.com/ntrrgc/jsvm/wiki)

# License

All files contained within this repository are licensed under the terms of the [Mozilla Public License 2.0](LICENSE.txt) unless stated otherwise.