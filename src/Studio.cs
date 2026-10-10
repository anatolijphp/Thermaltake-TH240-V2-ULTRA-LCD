// The UI-independent part of TT LCD Studio, shared by the Windows (WPF, App.cs) and Linux (GTK, src/gtk) front ends:
// the engine, settings changes and saving. Front ends call these from their UI thread and marshal events back to it.
using System;
using System.Threading;

namespace TtLcd
{
    class StudioCore
    {
        public readonly Engine Engine;
        public readonly IPlatform Platform;
        readonly Timer saveTimer;
        readonly object saveLock = new object();

        public StudioCore(IPlatform platform)
        {
            Platform = platform;
            Engine = new Engine(Config.Load());
            Engine.Start();
            saveTimer = new Timer(_ => SaveNow(), null, Timeout.Infinite, Timeout.Infinite);
        }

        public Config Config { get { return Engine.Config; } }

        // Settings are immutable snapshots: edit a copy, hand it to the engine, save shortly after the last change.
        public void Change(Action<Config> edit)
        {
            var c = Engine.Config.Clone();
            edit(c);
            Engine.Config = c;
            saveTimer.Change(700, Timeout.Infinite);
        }

        public void SaveNow()
        {
            saveTimer.Change(Timeout.Infinite, Timeout.Infinite);
            lock (saveLock) try { Engine.Config.Save(); } catch (Exception) { }
        }

        public void Shutdown()
        {
            SaveNow();
            Engine.Stop();
        }
    }
}
