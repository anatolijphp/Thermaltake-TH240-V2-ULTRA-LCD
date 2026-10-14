// A control shows the user's change at once; the app's next state confirms it (or puts it back).
import { useEffect, useState } from 'react';

export function useOptimistic(value) {
  const [local, setLocal] = useState(null);
  useEffect(() => { setLocal(null); }, [value]);
  return [local === null ? value : local.v, (v) => setLocal({ v })];
}
