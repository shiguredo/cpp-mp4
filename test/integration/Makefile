.PHONY: all get_input_files test update_input_check update_output_check

all: test

get_input_files:
	[ -f input/Big_Buck_Bunny_360_10s_1MB.mp4 ] || curl -o input/Big_Buck_Bunny_360_10s_1MB.mp4 https://test-videos.co.uk/vids/bigbuckbunny/mp4/h264/360/Big_Buck_Bunny_360_10s_1MB.mp4 # CC BY 3.0 https://creativecommons.org/licenses/by/3.0/ (c) copyright 2008, Blender Foundation / www.bigbuckbunny.org
	[ -f input/sample-5s.mp4  ] || curl -o input/sample-5s.mp4 https://samplelib.com/lib/download/mp4/sample-5s.mp4 # "Below are sample videos available for download with no license restrictions." -- https://samplelib.com/sample-mp4.html
	sha224sum -c input/check

test: get_input_files mux_test dump_test
	sha224sum -c output/check

mux_test:
	faketime -f '2020-12-01 00:00:00' ../../release/mp4-muxer opus -f output/opus_test.mp4 --opus resources/opus.csv
	faketime -f '2020-12-01 00:00:00' ../../release/mp4-muxer opusvp9 -f output/opusvp9_test.mp4 --opus resources/opus.csv --vp9 resources/vp9.csv
	faketime -f '2020-12-01 00:00:00' ../../release/mp4-muxer opusvp9_faststart -f output/opusvp9_faststart_test.mp4 --opus resources/opus.csv --vp9 resources/vp9.csv
	faketime -f '2020-12-01 00:00:00' ../../release/mp4-muxer mp3vp8 -f output/mp3vp8_test.mp4 --mp3 resources/mp3.csv --vp8 resources/vp8.csv
	faketime -f '2020-12-01 00:00:00' ../../release/mp4-muxer aacvp9 -f output/aacvp9_test.mp4 --aac resources/aac.csv --vp9 resources/vp9.csv
	faketime -f '2020-12-01 00:00:00' ../../release/mp4-muxer aacvp9_faststart -f output/aacvp9_faststart_test.mp4 --aac resources/aac.csv --vp9 resources/vp9.csv

dump_test:
	for f in input/*.mp4; do \
		base=$$(basename $${f}); \
		../../release/mp4-tool dump -f $${f} > output/$${base}.dump; \
	done


update_output_check:
	sha224sum output/*.mp4 output/*.dump > output/check

update_input_check:
	sha224sum input/*.mp4 > input/check
