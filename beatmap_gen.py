import sys
import random
from pydub.utils import mediainfo

def get_audio_length_ms(audio_path):
    info = mediainfo(audio_path)
    return int(float(info['duration']) * 1000)

def main():
    if len(sys.argv) < 3:
        print("Usage: python gen_beatmap.py <audiofile> <outputfile>")
        sys.exit(1)

    audio_path = sys.argv[1]
    output_path = sys.argv[2]
    bpm = 120  # 기본 BPM, 필요시 랜덤/입력 가능

    length_ms = get_audio_length_ms(audio_path)
    print(f"Audio length: {length_ms} ms")

    # 헤더 작성
    with open(output_path, "w") as f:
        f.write(f"@testbgm {length_ms} {audio_path}\n")
        f.write(f"#{bpm}\n")

        # 비트 수는 대략 음악 길이 / 500ms (2개/초)로 설정
        num_notes = length_ms // 500
        times = sorted(random.sample(range(1000, length_ms-1000), num_notes))
        for t in times:
            lane = random.randint(0, 3)
            f.write(f"{t} {lane}\n")

if __name__ == "__main__":
    main()