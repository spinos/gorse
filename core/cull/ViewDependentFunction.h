/*
 *  ViewDependentFunction.h
 *  aloe
 *
 *  view-frustum cull and level-of-detail select
 *
 */

#ifndef ALO_VIEW_DEPENDENT_FUNCTION_H
#define ALO_VIEW_DEPENDENT_FUNCTION_H

namespace alo {

class ViewFrustumCull;
class VisibleDetail;
class PerspectiveCamera;
class AFrustum;
class VisibilityState;
class BVHPrimitive;
class BoundingBox;

class ViewDependentFunction {

 	ViewFrustumCull *m_culler;
    VisibleDetail *m_details;
    bool m_freezeView;

public:

	ViewDependentFunction();
	virtual ~ViewDependentFunction();

protected:

	void initializeDetails();
	bool updateView(const PerspectiveCamera &camera, const AFrustum &frustum);

	void setFreezeView(bool x);
	const bool &freezeView() const;

	ViewFrustumCull *culler();
	VisibleDetail *details();

	const VisibilityState &visibility(int i) const;

	void clearBvh();
	void addBvhPrimitive(const BVHPrimitive &x);
	void buildBvh();

	const BoundingBox &bvhAabb() const;

private:

};

}

#endif
