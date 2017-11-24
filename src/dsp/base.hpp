
namespace bogaudio {
namespace dsp {

struct Generator {
	Generator() {}
	virtual ~Generator() {}

	virtual float next() = 0;
};

} // namespace dsp
} // namespace bogaudio
