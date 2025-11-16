struct Button {
    int x;
    int xwidth;
    int y;
    int ywidth;
    std::string name;

    void (*mousePressed) ();
};