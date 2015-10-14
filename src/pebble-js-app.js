//var initialized = false;
var options = {};

/*Pebble.addEventListener("ready", function() {
  //console.log("ready called!");
  initialized = true;
});*/

Pebble.addEventListener("showConfiguration", function(e) {
  //console.log("showing configuration");
  Pebble.openURL('http://pebble.hogo.cz/bar_watch/configurable.html?'+encodeURIComponent(JSON.stringify(options)));
  //Pebble.openURL('http://pebble.hogo.cz/bar_watch/configurable.html');
});

Pebble.addEventListener("webviewclosed", function(e) {
  //console.log("configuration closed");
    options = JSON.parse(decodeURIComponent(e.response));
    //console.log("Options = " + JSON.stringify(options));    
    
    var backgrnd = parseInt(options.backgrnd);
    var chour = parseInt(options.chour);
    var cminute = parseInt(options.cminute);
    var bluetooth = parseInt(options.bluetooth);
    var bluetooth_grid = parseInt(options.bluetooth_grid);
    var battery = parseInt(options.battery);
    
    var font;
    var pminute;
    var margin;
    var grid;    
    
    if (options.font_light === true) {font = 0;}
    if (options.font_normal === true) {font = 1;}
    
    if (options.pminute_down === true) {pminute = 0;}
    if (options.pminute_right === true) {pminute = 1;}
    if (options.pminute_diagonal === true) {pminute = 2;}
    if (options.pminute_random === true) {pminute = 3;}
    if (options.pminute_cycle === true) {pminute = 4;}
    
    if (options.margin_min === true) {margin = 0;}
    if (options.margin_low === true) {margin = 1;}
    if (options.margin_middle === true) {margin = 2;}
    if (options.margin_high === true) {margin = 3;}
    if (options.margin_max === true) {margin = 4;}    
    
    if (options.grid_off === true) {grid = 0;}
    if (options.grid_short === true) {grid = 1;}
    if (options.grid_long === true) {grid = 2;}
    if (options.grid_full === true) {grid = 3;}    

      var configuration = {
        "KEY_FONT": font,
        "KEY_MARGIN": margin,
        "KEY_BACKGRND": backgrnd,
        "KEY_BLUETOOTH": bluetooth,
        "KEY_BLUETOOTH_GRID": bluetooth_grid,
        "KEY_BATTERY": battery,
        "KEY_CMINUTE": cminute,
        "KEY_CHOUR": chour,
        "KEY_PMINUTE": pminute,
        "KEY_GRID": grid
      };  
    Pebble.sendAppMessage(configuration);
});