node {
    font-size: 10;
    font-weight: bold;
    font-variant: small-caps;
    font-family: cantarell;
}

node|z5-10[place=country] {
    text-color: #4A90D9;
    text: name;
}

node|z5-10[place=sea] {
    text-color: #770000;
    text: name;
}

/* This applies to the Map as a whole, renders first */
canvas {
    fill-color: #FAEBD7;
}


/* This applies to all lines drawn */
way {
    color: #777777;
    width: 1;
    linecap: round;
}

/* All lines with the tag 'highway' */


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

way|z0-7[highway] {
   width: 1;
   color: #777777;
   casing-width: 0;
}

way[highway=service] {
   width: 2;
}

way[highway=living_street] {
   width: 3;
}

way[highway=pedestrian] {
   color: #dddddd;
}

/* Applies to all lines drawn that passes these tests */
way[highway=motorway] {
    casing-width: 3;
    casing-color: #ff9966;
    color: #ffff00;
    width: 2;
}

way|z15-18[highway=motorway] {
    casing-width: 0.95;
    casing-color: #774433;
    color: #ff9966;
    width: 5;
}

way[highway=motorway_link] {
    casing-width: 0.95;
    casing-color: #774433;
    color: #ff9966;
    width: 3;
}

way[highway=trunk], way[highway=trunk_link] {
    casing-width: 0.95;
    color: #ff9966;
    casing-color: #bb5500;
    width: 5;
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
    casing-width: 0;
}

way[highway=cycleway] {
    color: #000077;
    width: 1;
    dashes: 2, 2;
    casing-width: 0;
}

way[highway=path] {
    color: #000000;
    width: 0.5;
    dashes: 5, 4;
    casing-width: 0;
}

way[highway=steps] {
    color: #a00000;
    width: 2;
    dashes: 1, 1;
    casing-width: 0;
}

way[highway=track] {
    color: #826a52;
    width: 1;
    dashes: 8, 8;
    casing-width: 0;
}

way[railway=rail] {
    color: #ffffff;
    width: 2;
    casing-color: #777777;
    casing-width: 1;
    dashes: 7, 10;
}

way[railway=tram], way[railway=lightrail] {
    color: #000000;
    width: 2;
    casing-width: 0;
}

way[railway=preserved], way[railway=narrow_gauge] {
   width: 1;
}

area {
    color: #000000;
    fill-color: #FAEBD7;
    width: 1;
}

area|z14-19 {
    text: name;
    text-color: #222233;
    font-size: 8;
    font-family: cantarell;
    font-weight: bold;
}

area[landuse=park], area[landuse=playground], area[landuse=pitch],
area[leisure=park], area[leasure=playground], area[leisure=pitch] {
    fill-color: #ABCA8A;
    color: #ABCA8a;
}

area[landuse=forest], area[landuse=wood], area[landuse=scrub],
area[natural=wood] {
    fill-color: #56A900;
    color: #56A900;
    z-index: 1; /* z-index 0 is baseline */
}

area[water], way[water], area[natural=water] {
    color: #4A90D9;
    fill-color: #4A90D9;
}

area[building] {
    color: #777777;
    fill-color: #C7B8A4;
}

area[amenity=parking] {
    color: #ffff77;
    fill-color: #C7B8A4;
}

area[highway=pedestrian] {
    color: #dddddd;
    fill-color: #C7B8A4;
}

/*
way[is_bridge=yes] {
    color: #333333;
    width: 2;
}
*/

way[is_tunnel=yes] {
    color: #EEEEEC;
    width: 2;
    dashes: 3, 3;
}



