JSVM builds Duktape with the following configuration options:

    OPTS=(
        -DDUK_USE_FATAL_HANDLER=JSVMDuktapeFatalErrorHandler
        -DDUK_USE_CPP_EXCEPTIONS
        --fixup-line 'extern void JSVMDuktapeFatalErrorHandler(void*, const char *msg);' \
    )

To update to a new version of Duktape:

1. Checkout the repo:

       git clone https://github.com/svaarala/duktape duktape
       cd duktape

2. Configure, passing the options mentioned above.

       python2 tools/configure.py --output-directory=jsvm-duk "${OPTS[@]}"
       
3. Rename `duktape.c` so that it builds as C++.

       mv jsvm-duk/duktape.{c,cpp}
       
4. Copy the contents of `jsvm-duk` back into this directory.