/* SoundWaveView */

#import <Cocoa/Cocoa.h>

struct SoundWaveRenderer;
@interface SoundWaveView : NSView
{
	NSRect skullRect;
	NSBitmapImageRep *bits;
	NSBitmapImageRep *skullBits;
	struct SoundWaveRenderer *renderer;
	NSDate* startDate;
}
@end
