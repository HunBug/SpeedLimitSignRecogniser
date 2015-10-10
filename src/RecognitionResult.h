/*
 * RecognitionResult.h
 *
 *  Created on: 2015. okt. 5.
 *      Author: a.strack
 */

#ifndef RECOGNITIONRESULT_H_
#define RECOGNITIONRESULT_H_

#include <opencv2/core.hpp>
#include <boost/serialization/serialization.hpp>

namespace slsr {

class RecognitionResult {
public:
	static RecognitionResult createNotFoundResult();
	static RecognitionResult createFoundResult(cv::Rect signPosition,
			unsigned char speedLimit);
	virtual ~RecognitionResult();

	RecognitionResult();

	bool isSignFound() const {
		return m_signFound;
	}

	const cv::Rect& getSignPosition() const {
		return m_signPosition;
	}

	unsigned char getSpeedLimit() const {
		return m_speedLimit;
	}

private:
	RecognitionResult(bool signFound, cv::Rect signPosition,
			unsigned char speedLimit);
	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version) {
		ar & m_signFound;
		ar & m_speedLimit;
		ar & m_signPosition.x;
		ar & m_signPosition.y;
		ar & m_signPosition.width;
		ar & m_signPosition.height;
	}

	cv::Rect m_signPosition;
	unsigned char m_speedLimit;
	bool m_signFound;
};

} /* namespace slsr */

#endif /* RECOGNITIONRESULT_H_ */
