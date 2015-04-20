canvas {
    fill-color: #FAEBD7;
}

way {
    color: #777777;
    width: 1;
    linecap: round;
}

way[highway] {
    color: #ffffff;
    linecap: round;
    casing-linecap: round;
    linejoin: miter;
    casing-linejoin: miter;
    casing-width: 1;
    width: 5;
    casing-color: #C7B8A4;
}


way[highway=motorway], way[highway=motorway_link],
way[highway=trunk], way[highway=trunk_link] {
    casing-width: 0.95;
    color: #ff9966;
    casing-color: #bb5500;
    width: 7;
}
way[highway=primary], way[highway=primary_link] {
    casing-width: 0.8;
    width: 2;
    color: #ffbb88;
    casing-color: #cc6600;
}

way[highway=footway] {
    color: #770000;
    width: 1;
    dashes: 2, 2;
}

way[highway=cycleway] {
    color: #000077;
    width: 1;
    dashes: 2, 2;
}

area {
    color: #FAEBD7;
    fill-color: #FAEBD7;
    width: 1;
}

area[landuse=park], area[landuse=playground], area[landuse=pitch] {
    fill-color: #73D216;
    color: #73D216;
}

area[landuse=wood], area[landuse=scrub] {
    fill-color: #0D6C00;
    color: #0D6C00;
    z-index: 1;
}

area[water], way[water], area[natural=water] {
    color: #4A90D9;
    fill-color: #4A90D9;
}

area[building] {
    color: #777777;
    fill-color: #C7B8A4;
}

way[is_bridge=yes] {
    color: #333333;
    width: 2;
}

way[is_tunnel=yes] {
    color: #333333;
    width: 2;
    dashes: 3, 3;
}



