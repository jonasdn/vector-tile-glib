@import("roads.mapcss");

node {
    font-size: 10;
    font-weight: bold;
    font-variant: small-caps;
    font-family: cantarell;
}

node|z5-10[place=country] {
    text-color: #4A90D9;
    text: name;
    text-halo-radius: 2;
    text-halo-color:white;
}

node|z5-10[place=sea] {
    text-color: #770000;
    text: name;
    text-halo-radius: 2;
    text-halo-color:white;
}

node|z5-13[place=city] {
    font-family: cantarell;
    font-weight: bold;
    text-color: #000000;
    font-size: 14;
    text: name;
    text-halo-radius: 2;
    text-halo-color: white;
}


/* This applies to the Map as a whole, renders first */
canvas {
    fill-color: #FAEBD7;
}

area {
    color: #000000;
    fill-color: #FAEBD7;
    width: 1;
}

area|z16-19[building] {
    text: name;
    text-color: #222233;
    text-halo-color: white;
    text-halo-radius: 1;
    font-size: 8;
    font-family: cantarell;
}

area|z16-19[building] {
    text: name;
    text-color: #222233;
    text-halo-color: white;
    text-halo-radius: 1;
    font-size: 8;
    font-family: cantarell;
}

area|z16-19[landuse] {
    text: name;
    text-color: #222233;
    text-halo-color: white;
    text-halo-radius: 1;
    font-size: 8;
    font-family: cantarell;
}

area[landuse=park], area[landuse=playground], area[landuse=pitch],
area[leisure=park], area[leasure=playground], area[leisure=pitch],
area[landuse=grass], area[landuse=garden], area[landuse=golf_course],
area[landuse=cemetery], area[landuse=meadow], area[landuse=common],
area[landuse=sports_centre], area[landuse=farmland], area[landuse=farm],
area[landuse=farmyard], area[landuse=allotments], area[landuse=village_green] {
    fill-color: #cfe1bd;
    color: #cfe1bd;
}

area[landuse=forest], area[landuse=wood], area[landuse=scrub],
area[natural=wood], area[landuse=nature_reserve] {
    fill-color: #ABCA8a;
    color: #ABCA8a;
    z-index: 1; /* z-index 0 is baseline */
}

area[landuse=wetland] {
    fill-color: #7BB26A;
}

area[water], way[water], area[natural=water] {
    color: #4A90D9;
    fill-color: #4A90D9;
}

area|z13-16[building] {
    color: #C7B8A4;
    fill-color: #C7B8A4;
}

area|z17[building] {
    color: #777777;
    fill-color: #C7B8A4;
}

area[landuse=parking] {
    color: #fff59f;
    fill-color: #fff59f;
}

area[landuse=residential], area[landuse=industrial] {
    fill-color: #e6dbcb;
    color: #e6dbcb;
}

area[landuse=quarry] {
    fill-color: #cdcbcb;
    color: #cdcbcb;
}

area[amenity=parking] {
    color: #ffff77;
    fill-color: #C7B8A4;
}

area[highway=pedestrian] {
    color: #dddddd;
    fill-color: #C7B8A4;
}

