const VTile = imports.gi.VectorTileGlib;
const Cairo = imports.gi.cairo;
const Mainloop = imports.mainloop;

let mapCSS = new VTile.MapCSS();
mapCSS.load('all.mapcss');

let tags = { highway: 'motorway' };
let style = mapCSS.get_style(VTile.MapCSSSelectorType.WAY,
                             tags,
                             14);
let color = style.get_color('color');
let casing_color = style.get_color('casing-color');
log('motorway:');
log('color: ' + color.r + ', ' + color.g + ', ' + color.b);
log('width: ' + style.get_num('width'));
log('casing-width: ' + style.get_num('width'));
log('casing-color: ' + casing_color.r + ', ' + casing_color.g + ', ' + casing_color.b);

let mapbox = new VTile.Mapbox();
mapbox.set_stylesheet(mapCSS);

let surface = new Cairo.ImageSurface(Cairo.FORMAT.ARGB32, 256, 256);

mapbox.render_async(function() {
    log('rendered!');
    Mainloop.quit('test-bindings');
});

Mainloop.run('test-bindings');
