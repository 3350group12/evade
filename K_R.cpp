int main(int argc, char *argv[])
{
    ...
    while (x == y) {
        something();
        somethingelse();

        if (some_error)
            do_correct();
        else
            continue_as_usual();
    }

    finalthing();
    //...
}
//--------------------------------------------------------------------
//Example of if-else

    if (variable < 100) {
        variable = 100;
    } else {
        variable *= 0.99;
    }
