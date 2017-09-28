NUMBERS := f0.h f1.h f2.h f3.h f4.h f5.h f6.h f7.h f8.h f9.h colon.h slash.h l0.h l1.h l2.h l3.h l4.h l5.h l6.h l7.h l8.h l9.h

all: off.h on_i.h birthday.h blueon.h haloween.h independance_day.h off.h redon.h thanksgiving.h valentines.h candy_cane.h christmas_tree.h firework.h santa.h $(NUMBERS)

.PHONY: all

# also: -ordered-dither o8x8,32,64,32
%.rgb: %.png
	convert $< -depth 8 -ordered-dither threshold,32,64,32 -gamma 3.0 $@

%.h: %.rgb rgb-to-c
	./rgb-to-c $* <$< >$@
