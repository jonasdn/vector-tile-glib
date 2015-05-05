const VTile = imports.gi.VectorTileGlib;
const Cairo = imports.cairo;
const Mainloop = imports.mainloop;

let mapCSS = new VTile.MapCSS();
mapCSS.load('all.mapcss');

let tags = { highway: 'motorway' };
let style = mapCSS.get_style(VTile.MapCSSSelectorType.WAY,
                             tags,
                             14);
let color = style.get_color('color');
let c_color = style.get_color('casing-color');
let dash = style.get_dash('dashes');

log('motorway:');
log('color: ' + color.r + ', ' + color.g + ', ' + color.b);
log('width: ' + style.get_num('width'));
log('casing-width: ' + style.get_num('width'));
log('casing-color: ' + c_color.r + ', ' + c_color.g + ', ' + c_color.b);
log('dash: ' + dash.num_dashes + ' - ' + dash.dashes[0] + ', ' + dash.dashes[1]);

let mapbox = new VTile.Mapbox({ tile_size: 256,
                                zoom_level: 17 });
mapbox.load_from_file('test.mapbox');
mapbox.set_stylesheet(mapCSS);

let surface = new Cairo.ImageSurface(Cairo.Format.ARGB32, 256, 256);
let cr = new Cairo.Context(surface);

mapbox.render_async(cr, function() {
    surface.writeToPNG('test.png');
    Mainloop.quit('test-bindings');
});

Mainloop.run('test-bindings');
