// Display modes: clicking the selected reading again deselects it and hides its text style (2026-10-15).
// The reading chips send {t:'select'} on click (LineEditor.jsx); this sends the same.
window.ttlcd.page('modes');
T.send({ t: 'set', k: 'Mode', v: 'Clock' }); // the Clock tile (ModeTiles.jsx); the text style is a Clock option
const select = () => T.send({ t: 'select', list: 'Clock', id: 0 });
const shown = () => !!T.$('.text-style');
await T.wait(500);
const first = shown();
select();
await T.wait(500);
const once = shown();
select();
await T.wait(500);
const twice = shown();
T.ok('a click selects the reading (text style shows), a second click deselects it (text style hides)',
  once !== twice && (once || twice) && first !== once,
  `before: ${first}, after one click: ${once}, after two: ${twice}`);
select();
await T.wait(500);
T.ok('a third click selects it again', shown() === once, `shown: ${shown()}`);
