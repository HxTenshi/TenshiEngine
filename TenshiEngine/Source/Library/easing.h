#pragma once

//http://easings.net/ja

//イージング関数
namespace Easing
{
	inline double InQuad(double t, double totaltime, double max, double min)
	{
		max -= min;
		t /= totaltime;
		return max*t*t + min;
	}
	inline double OutQuad(double t, double totaltime, double max, double min)
	{
		max -= min;
		t /= totaltime;
		return -max*t*(t - 2) + min;
	}
	inline double InOutQuad(double t, double totaltime, double max, double min)
	{
		max -= min;
		t /= totaltime;
		if (t / 2 < 1)
			return max / 2 * t * t + min;
		--t;
		return -max * (t * (t - 2) - 1) + min;
	}
	inline double InCubic(double t, double totaltime, double max, double min)
	{
		max -= min;
		t /= totaltime;
		return max * t*t*t + min;
	}
	inline double OutCubic(double t, double totaltime, double max, double min)
	{
		max -= min;
		t = t / totaltime - 1;
		return max * (t*t*t + 1) + min;
	}
	inline double InOutCubic(double t, double totaltime, double max, double min)
	{
		max -= min;
		t /= totaltime;
		if (t / 2 < 1)
			return max / 2 * t*t*t + min;
		t -= 2;
		return max / 2 * (t*t*t + 2) + min;
	}
	inline double InQuart(double t, double totaltime, double max, double min)
	{
		max -= min;
		t /= totaltime;
		return max * t*t*t*t + min;
	}
	inline double OutQuart(double t, double totaltime, double max, double min)
	{
		max -= min;
		t = t / totaltime - 1;
		return -max*(t*t*t*t - 1) + min;
	}
	inline double InOutQuart(double t, double totaltime, double max, double min)
	{
		max -= min;
		t /= totaltime;
		if (t / 2 < 1)
			return max / 2 * t*t*t*t + min;
		t -= 2;
		return -max / 2 * (t*t*t*t - 2) + min;
	}
	inline double InQuint(double t, double totaltime, double max, double min)
	{
		max -= min;
		t /= totaltime;
		return max*t*t*t*t*t + min;
	}
	inline double OutQuint(double t, double totaltime, double max, double min)
	{
		max -= min;
		t = t / totaltime - 1;
		return max*(t*t*t*t*t + 1) + min;
	}
	inline double InOutQuint(double t, double totaltime, double max, double min)
	{
		max -= min;
		t /= totaltime;
		if (t / 2 < 1)
			return max / 2 * t*t*t*t*t + min;
		t -= 2;
		return max / 2 * (t*t*t*t*t + 2) + min;
	}
	inline double InSine(double t, double totaltime, double max, double min)
	{
		max -= min;
		return -max*cos(t*1.570796326794896 / totaltime) + max + min;
	}
	inline double OutSine(double t, double totaltime, double max, double min)
	{
		max -= min;
		return max * sin(t*1.570796326794896 / totaltime) + min;
	}
	inline double InOutSine(double t, double totaltime, double max, double min)
	{
		max -= min;
		return -max / 2 * (cos(t*3.141592653589793 / totaltime) - 1) + min;
	}
	inline double InExp(double t, double totaltime, double max, double min)
	{
		max -= min;
		return t == 0.0 ? min : max*pow(2, 10 * (t / totaltime - 1)) + min;
	}
	inline double OutExp(double t, double totaltime, double max, double min)
	{
		max -= min;
		return t == totaltime ? max + min : max*(-pow(2, -10 * t / totaltime) + 1) + min;
	}
	inline double InOutExp(double t, double totaltime, double max, double min)
	{
		if (t == 0.0)
			return min;
		if (t == totaltime)
			return max;
		max -= min;
		t /= totaltime;

		if (t / 2 < 1)
			return max / 2 * pow(2, 10 * (t - 1)) + min;
		--t;
		return max / 2 * (-pow(2, -10 * t) + 2) + min;

	}
	inline double InCirc(double t, double totaltime, double max, double min)
	{
		max -= min;
		t /= totaltime;
		return -max*(sqrt(1 - t*t) - 1) + min;
	}
	inline double OutCirc(double t, double totaltime, double max, double min)
	{
		max -= min;
		t = t / totaltime - 1;
		return max*sqrt(1 - t*t) + min;
	}
	inline double InOutCirc(double t, double totaltime, double max, double min)
	{
		max -= min;
		t /= totaltime;
		if (t / 2 < 1)
			return -max / 2 * (sqrt(1 - t*t) - 1) + min;
		t -= 2;
		return max / 2 * (sqrt(1 - t*t) + 1) + min;
	}
	inline double InBack(double t, double totaltime, double max, double min, double s)
	{
		max -= min;
		t /= totaltime;
		return max*t*t*((s + 1)*t - s) + min;
	}
	inline double OutBack(double t, double totaltime, double max, double min, double s)
	{
		max -= min;
		t = t / totaltime - 1;
		return max*(t*t*((s + 1)*t*s) + 1) + min;
	}
	inline double InOutBack(double t, double totaltime, double max, double min, double s)
	{
		(void)totaltime;
		max -= min;
		s *= 1.525;
		if (t / 2 < 1)
		{
			return max*(t*t*((s + 1)*t - s)) + min;
		}
		t -= 2;
		return max / 2 * (t*t*((s + 1)*t + s) + 2) + min;
	}
	inline double OutBounce(double t, double totaltime, double max, double min)
	{
		max -= min;
		t /= totaltime;

		if (t < 1 / 2.75)
			return max*(7.5625*t*t) + min;
		else if (t < 2 / 2.75)
		{
			t -= 1.5 / 2.75;
			return max*(7.5625*t*t + 0.75) + min;
		}
		else if (t< 2.5 / 2.75)
		{
			t -= 2.25 / 2.75;
			return max*(7.5625*t*t + 0.9375) + min;
		}
		else
		{
			t -= 2.625 / 2.75;
			return max*(7.5625*t*t + 0.984375) + min;
		}
	}
	inline double InBounce(double t, double totaltime, double max, double min)
	{
		return max - OutBounce(totaltime - t, totaltime, max - min, 0) + min;
	}
	inline double InOutBounce(double t, double totaltime, double max, double min)
	{
		if (t < totaltime / 2)
			return InBounce(t * 2, totaltime, max - min, max)*0.5 + min;
		else
			return OutBounce(t * 2 - totaltime, totaltime, max - min, 0)*0.5 + min + (max - min)*0.5;
	}
	inline double Linear(double t, double totaltime, double max, double min)
	{
		return (max - min)*t / totaltime + min;
	}
}