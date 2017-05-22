# SimpleQWT version 6.0
SimpleQWT is targeted at projects that want only core qwt, with fixes and improvements.

- targets reduced computational and memory footprint compared to full-fat version
- fixes for stable axes/canvas while panning/zooming
- fixes and production-ready antialiasing
- some fixes for plot renderer for wysiwyg
- forked from QWT "6.0.0-svn"
- some fixes pulled from 6.1
- Qt5 fixes, legacy cleanup, simplification and streamlining

Initial version as used by NSpec scientific software.


# Q/A:
- Why not QWT 6.1?

It is slower and uses more memory according to my tests (mostly Plot/Slider).
Quick review didn't reveal the single culprit for that change.

- Why not merge it with regular QWT or fix full version?

Full QWT is much larger than just core, i don't have enough time for that.
Some changes are very invasive and not possible there at all.
Some simplifications are only possible after removing more elaborate functions.
I hope most minor fixes and legacy cleanups can be ported back to original version, 
but that's a lot of work too due to 6.1 changes.
