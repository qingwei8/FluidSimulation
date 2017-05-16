#ifndef SPHKERNELS_H
#define SPHKERNELS_H

#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <Eigen/Dense>
#include "Common.h"

#define NO_DISTANCE_TEST

namespace hiveFluidSimulation
{
	class CCubicKernel
	{
	protected:
		static double m_radius;
		static double m_k;
		static double m_l;
		static double m_W_zero;

	public:
		static double getRadius() { return m_radius; }
		static void setRadius(double val)
		{
			m_radius = val;
			static const double pi = static_cast<double>(M_PI);

			const double h3 = m_radius*m_radius*m_radius;
			m_k = 8.0 / (pi*h3);
			m_l = 48.0 / (pi*h3);
			m_W_zero = W(Eigen::Matrix<float, 3, 1>(0.0, 0.0, 0.0));
		}

	public:
		//static unsigned int counter;
		static double W(const Eigen::Matrix<float, 3, 1> &r)
		{
			//counter++;
			double res = 0.0;
			const double rl = r.norm();
			const double q = rl/m_radius;
#ifndef NO_DISTANCE_TEST
			if (q <= 1.0)
#endif
			{
				if (q <= 0.5)
				{
					const double q2 = q*q;
					const double q3 = q2*q;
					res = m_k * (6.0*q3-6.0*q2+1.0);
				}
				else
				{
					res = m_k * (2.0*pow(1.0-q,3));
				}
			}
			return res;
		}

		static Eigen::Matrix<float, 3, 1> gradW(const Eigen::Matrix<float, 3, 1> &r)
		{
			Eigen::Matrix<float, 3, 1> res;
			const float rl = r.norm();
			const float q = rl / m_radius;
#ifndef NO_DISTANCE_TEST
			if (q <= 1.0)
#endif
			{
				if (rl > 1.0e-6)
				{
					const Eigen::Matrix<float, 3, 1> gradq = r * ((float) 1.0 / (rl*m_radius));
					if (q <= 0.5)
					{
						res = m_l*q*((float) 3.0*q - (float) 2.0)*gradq;
					}
					else
					{
						const float factor = 1.0 - q;
						res = m_l*(-factor*factor)*gradq;
					}
				}
			}
#ifndef NO_DISTANCE_TEST
 			else
 				res.zero();
#endif

			return res;
		}

		static float W_zero()
		{
			return m_W_zero;
		}
	};
}

#endif