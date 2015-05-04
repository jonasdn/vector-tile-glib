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
    text-halo-color:#ffffff;
}

node|z5-10[place=sea] {
    text-color: #770000;
    text: name;
    text-halo-radius: 2;
    text-halo-color:#ffffff;
}

node|z5-13[place=city] {
    font-family: cantarell;
    font-weight: bold;
    text-color: #000000;
    font-size: 14;
    text: name;
    text-halo-radius: 2;
    text-halo-color: #ffffff;
}

/* This applies to the Map as a whole, renders first */
canvas {
    fill-color: #FAEBD7;
}

/* Roads */

/* Generic style for all roads */
way|z8-17[highway] {
    color: #d2ccc3;
    casing-color: #c7b8a4;
    casing-width: 1;
}

way|z8-17[highway][is_bridge=yes] {
    casing-color: #b0a696;
}

/* type specific way overrides*/
way|z8-17[highway=motorway],
way|z8-17[highway=motorway_link] {
    color: #ffaf60;
    casing-color: #f57900;
    z-index: 7;
}

way|z8-17[highway=motorway][is_bridge=yes] {
    casing-color: #cc6500;
}

way|z9-17[highway=trunk],
way|z9-17[highway=trunk_link],
way|z9-17[highway=primary],
way|z9-17[highway=primary_link] {
    color: #ffea40;
    casing-color: #c4af00;
    z-index: 6;
}

way|z9-17[highway=trunk][is_bridge=yes],
way|z9-17[highway=primary][is_bridge=yes] {
    casing-color: #d3bc00;
}

/* Grow roads on zoom */
way|z9[highway=trunk],
way|z9[highway=primary] {
    width: 1;
}

way|z9[highway=motorway],
way|z10[motorway_link],
way|z10[highway=trunk],
way|z10[highway=primary],
way|z11[highway=trunk_link],
way|z11[highway=primary_link] {
    width: 2;
}

way|z10[highway=motorway],
way|z11[highway=motorway_link],
way|z11[highway=trunk],
way|z11[highway=primary],
way|z12[highway=trunk_link],
way|z12[highway=primary_link] {
    width: 3;
}

way|z11-17[highway=motorway],
way|z12-17[highway=motorway_link],
way|z12-17[highway=trunk],
way|z12[highway=primary],
way|z13-17[highway=trunk_link],
way|z13-17[highway=primary_link] {
    width: 7;
}

way|z10[highway=secondary],
way|z11[highway=tertiary],
way|z12[highway=unclassified],
way|z12[highway=road],
way|z12-13[highway=residential],
way|z15[highway=service],
way|z15[highway=footway],
way|z15[highway=cycleway],
way|z15[highway=path],
way|z15[highway=pedestrian],
way|z15[highway=track],
way|z15[highway=living_street],
way|z15[highway=steps] {
    width: 1;
    casing-width: 0;
    z-index: 1;
}

way|z11[highway=secondary],
way|z12[highway=tertiary],
way|z13[highway=unclassified],
way|z13[highway=road],
way|z14[highway=residential],
way|z16[highway=service],
way|z16[highway=footway],
way|z16[highway=cycleway],
way|z16[highway=path],
way|z16[highway=pedestrian],
way|z16[highway=track],
way|z16[highway=living_street],
way|z16[highway=steps] {
    width: 1;
    color: #ffffff;
    casing-width: 1;
    z-index: 2;
}

way|z12[highway=secondary],
way|z13[highway=tertiary],
way|z14[highway=unclassified],
way|z14[highway=road],
way|z15[highway=residential],
way|z17[highway=service],
way|z17[highway=footway],
way|z17[highway=cycleway],
way|z17[highway=path],
way|z17[highway=pedestrian],
way|z17[highway=track],
way|z17[highway=living_street],
way|z17[highway=steps] {
    width: 3;
    color: #ffffff;
    z-index: 3;
}

way|z13[highway=secondary],
way|z14[highway=tertiary],
way|z15[highway=unclassified],
way|z15[highway=road],
way|z16[highway=residential] {
    width: 4;
    color: #ffffff;
    z-index: 4;
}

way|z14-17[highway=secondary],
way|z15-17[highway=tertiary],
way|z16-17[highway=unclassified],
way|z16[highway=road],
way|z17[highway=residential] {
    width: 7;
    color: #ffffff;
    z-index: 5;
}

way|z11-12[highway=residential] {
    opacity: 0;
}

way|z17[highway=residential] {
    text: name;
    text-color: #222233;
    text-halo-color: #ffffff;
    text-halo-radius: 1;
    font-size: 7;
    font-family: cantarell;
}

way|z14-17[highway!=residential] {
    text: name;
    text-color: #222233;
    text-halo-color: #ffffff;
    text-halo-radius: 1;
    font-size: 8;
    font-family: cantarell;
}

area {
    color: #000000;
    fill-color: #FAEBD7;
    width: 1;
}

area|z16-19[building] {
    text: name;
    text-color: #222233;
    text-halo-color: #ffffff;
    text-halo-radius: 1;
    font-size: 8;
    font-family: cantarell;
}

area|z16-19[landuse] {
    text: name;
    text-color: #222233;
    text-halo-color: #ffffff;
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

way[is_tunnel=yes] {
    color: #EEEEEC;
    width: 2;
    dashes: 3, 3;
}

/* Railways */
way[railway=rail] {
    color: #ffffff;
    width: 2;
    casing-color: #777777;
    casing-width: 1;
    dashes: 7, 10;
}
/*
way[railway=tram], way[railway=lightrail] {
    color: #888a85;
    casing-color: #888a85;
    casing-width: 1.5;
    casing-dashes: 1, 6;
    width: 2;
}
*/
