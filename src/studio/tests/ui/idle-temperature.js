// The Idle preview shows the coolant temperature as the cooler's idle screen draws it: an image the app renders
// with the firmware's fonts (idletemp/<n>.png, core/src/idle_text.rs), following the live reading (2026-10-15).
const status = window.ttlcd.status;
let temp = 31;
window.ttlcd.status = (s) => status(Object.assign({}, s, { idleTemp: temp })); // as if the cooler reported it
T.send({ t: 'idle', a: 'view', v: true });
await T.wait(1500);
let img = T.$('img.idle-temp');
T.ok('the Idle view shows the temperature image', !!img);
if (img) {
  T.ok('it is the drawing for the reading', img.getAttribute('src') === 'idletemp/31.png', img.getAttribute('src'));
  T.ok('the app serves it (480 x 480)', img.complete && img.naturalWidth === 480, `${img.naturalWidth} x ${img.naturalHeight}`);
}
temp = 32;
await T.wait(1500);
img = T.$('img.idle-temp');
T.ok('a new reading shows within a status update', !!img && img.getAttribute('src') === 'idletemp/32.png', img && img.getAttribute('src'));
temp = null;
await T.wait(1500);
T.ok('no reading: no temperature', !T.$('img.idle-temp'));
T.send({ t: 'idle', a: 'view', v: false });
