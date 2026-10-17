// The Pump display mode (2026-10-15): its tile selects it; its options choose the speed source (the CPU fan by
// default, any motherboard fan, or set by hand) and the LED light (red, or from Aura Sync); its frames keep coming.
window.ttlcd.page('modes');
await T.wait(500);
const tile = [...document.querySelectorAll('.tile')].find((t) => t.textContent.trim() === 'Pump');
T.ok('Display modes has a Pump tile', !!tile);
T.send({ t: 'set', k: 'Mode', v: 'Pump' }); // what the tile sends (setCfg)
await T.wait(1000);
const cfg = () => T.store().S.cfg;
const text = () => document.body.textContent;
T.ok('it becomes the mode', String(cfg().Mode) === 'Pump', cfg().Mode);
T.ok('the speed source is offered (the CPU fan is the automatic choice)', /Speed from/.test(text()) && (cfg().PumpFan !== '' || /CPU fan \(automatic\)/.test(text())));
const st = T.store().ST;
T.ok('the status says where the speed comes from', !!(st && st.pump), JSON.stringify(st && st.pump));
const fan = cfg().PumpFan;
T.send({ t: 'set', k: 'PumpFan', v: 'manual' });
await T.wait(800);
T.ok('set by hand: the rpm slider shows', /Pump rpm/.test(text()));
T.send({ t: 'set', k: 'PumpRpm', v: 1200 });
await T.wait(800);
T.ok('the set speed is stored for the renderer', cfg().PumpRpm === 1200, cfg().PumpRpm);
// the switch, whatever your settings have it at (checks never save): flip it, see it stored, flip it back
const aura = cfg().PumpAura;
T.ok('the light can follow Aura Sync', /From Aura Sync/.test(text()) && typeof aura === 'boolean');
T.send({ t: 'set', k: 'PumpAura', v: !aura });
await T.wait(800);
T.ok('switching it is stored', cfg().PumpAura === !aura);
T.send({ t: 'set', k: 'PumpAura', v: aura });
T.send({ t: 'set', k: 'PumpFan', v: fan });
// 2026-10-17: the view can be tilted up to 30 degrees in any direction: the pad shows where the eye is and says it
// in words; a value past 30 (e.g. from `ttlcd set`) is shown on the ring, as the renderer holds it there
const pad = T.$('#pump-view');
T.ok('a "View from" pad is offered', !!pad && /View from/.test(text()));
const view = [cfg().PumpViewX, cfg().PumpViewY];
T.send({ t: 'set', k: 'PumpViewX', v: 20 });
T.send({ t: 'set', k: 'PumpViewY', v: -10 });
await T.wait(800);
T.ok('the tilt is stored for the renderer', cfg().PumpViewX === 20 && cfg().PumpViewY === -10, `${cfg().PumpViewX},${cfg().PumpViewY}`);
T.ok('the pad says it in words', /20° right, 10° up/.test(text()));
const dot = pad && pad.querySelector('.dot');
T.ok('the dot sits right of and above the centre', !!dot && parseFloat(dot.style.left) > 50 && parseFloat(dot.style.top) < 50, dot && `${dot.style.left} ${dot.style.top}`);
T.send({ t: 'set', k: 'PumpViewX', v: 60 });
T.send({ t: 'set', k: 'PumpViewY', v: 0 });
await T.wait(800);
T.ok('past 30 it shows 30', /30° right/.test(text()) && Math.abs(parseFloat(dot.style.left) - 100) < 0.5, dot && dot.style.left);
T.send({ t: 'set', k: 'PumpViewX', v: 0 });
await T.wait(500);
T.ok('straight down', /Straight down/.test(text()));
T.send({ t: 'set', k: 'PumpViewX', v: view[0] });
T.send({ t: 'set', k: 'PumpViewY', v: view[1] });
// the live preview shows the frames the mode renders: they keep coming
const live = T.$('img.live');
const first = live && live.getAttribute('src');
await T.wait(1500);
T.ok('the preview keeps getting new frames', !!live && !!first && live.getAttribute('src') !== first, `${first} -> ${live && live.getAttribute('src')}`);
