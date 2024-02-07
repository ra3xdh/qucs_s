#include "mouseactions.h"
#include "qucsdoc.h"
#include "qucs.h"
#include "schematic.h"

QRect Schematic::modelRect()
{
    return QRect{ViewX1, ViewY1, ViewX2 - ViewX1, ViewY2 - ViewY1};
}

QRect Schematic::viewportRect() {
    return QRect{0, 0, viewport()->width(), viewport()->height()};
}

QPoint Schematic::modelToViewport(QPoint modelCoordinates)
{
    return modelToContents(modelCoordinates) - QPoint{ contentsX(), contentsY() };
}

QPoint Schematic::viewportToModel(QPoint viewportCoordinates)
{
    return contentsToModel(QPoint{ contentsX(), contentsY() } + viewportCoordinates);
}

QPoint Schematic::contentsToModel(const QPoint& coordinates)
{
    // Sizes in the model and contents are interconnected and obey the rule:
    //     <size on model plane> * <Scale> = <size on contents>
    //
    // Contents is a rectangle with (0, 0) at its top-left corner. Model plane
    // is rectangular area of abstract infinite plane, so model plane's top-left
    // corner may have any coordinates.
    //
    // To transform coordinates of a point on the contents to coordinates
    // of corresponding point on model plane:
    // 1. Adjust "contents" coordinates so that they become having the same scale
    //    the model plane has
    // 2. Adjust resulting coordinates so they become absolute coordinates
    //    in model plane

    // QPoint overrides operator /. It divides both coordinates on given value
    QPoint modelCoords = coordinates / Scale;

    modelCoords.setX(ViewX1 + modelCoords.x());
    modelCoords.setY(ViewY1 + modelCoords.y());
    return modelCoords;
}

QPoint Schematic::modelToContents(const QPoint& coordinates)
{
    // Model and contents sizes are interconnected and obey the rule:
    //     <size on model plane> * <Scale> = <size on contents>
    //
    // Contents is a rectangle with (0, 0) at its top-left corner. Model plane
    // is rectangular area of abstract infinite plane, so model plane's top-left
    // corner may have any coordinates.
    //
    // To transform coordinates of a point on the model plane to coordinates
    // of corresponding point on the contents:
    // 1. Adjust coordinates so that they become relative to model planes'
    //    top-left corner
    // 2. Adjust resulting coordinates so thay they become having the same scale
    //    as contents

    QPoint contentsCoords{coordinates.x() - ViewX1, coordinates.y() - ViewY1};
    contentsCoords *= Scale;
    return contentsCoords;
}

double Schematic::clipScale(double offeredScale)
{
    if (offeredScale > maxScale) {
        return maxScale;
    } else if (offeredScale < minScale) {
        return minScale;
    } else {
       return offeredScale;
    }
}

bool Schematic::shouldRender(const double& newScale, const QRect& newModelBounds, const QPoint& toBeDisplayed, const QPoint& viewportCoords) {
    const QRect currentModelBounds = modelRect();
    // This point currently displayed at "viewportCoords" of the viewport
    const QPoint currenlyDisplayed = viewportToModel(viewportCoords);
    return Scale != newScale || toBeDisplayed != currenlyDisplayed || currentModelBounds != newModelBounds;
}

double Schematic::renderModel(const double offeredScale, QRect newModel, const QPoint modelPoint, const QPoint viewportPoint)
{
    // This is the core method to render the schematic. It employs
    // an approach similar to MVC (model-view-controller).
    //
    // There is a "model plane" – a rectangle located somewhere in some
    // abstract cartesian coordinate system. This coordinate system has
    // its X-axis directed left-to-right and Y-axis top-to-bottom. Every element
    // of the schematic "lives" in this coordinate system inside the model
    // plane. The model plane is described by class properties ViewX1, ViewY1,
    // ViewX2, ViewY1.
    //
    // To see the schematic we need to "render" the model plane. To do so the
    // "view plane" is used. Technically it is the Q3ScrollView's "contents",
    // but in the abstraction being described its just another plane. In
    // contrast to the model plane, its top-left corner is always (0,0), but the
    // axes are directed the same way: left-to-right and top-to-bottom.
    //
    // The "scale" is the ratio between the view plane and model plane sizes.
    // When being rendered, the model is like being "projected" on the view
    // plane. Think of the film projector: the film ("model") could be displayed
    // on screens ("views") of different sizes by adjusting the lense ("scale").
    //
    // Finally there is a "viewport". Technically its the very Q3ScrollView
    // widget, but in the abstraction being described its the part of the
    // view which is currently observed by the user. Sometimes the view is
    // very large (when we "zoom in") and only the part of it could be observed.
    //
    // Summarizing everything said:
    // 1. The model plane is drawn on the view plane at some scale and then
    // a part of the view plane is observed by the user.
    // 2. <size in model plane> * <Scale> = <size in view plane>

    // DO NOT alter model bounds or scale and DO NOT call resizeContens() outside
    // of this method. It will break the state and lead to hard-to-find bugs.
    // Pass the desired model bounds or scale as the argument to this method.

    assert(modelPoint.x() >= newModel.left() && modelPoint.x() <= newModel.right());
    assert(modelPoint.y() >= newModel.top() && modelPoint.y() <= newModel.bottom());
    assert(viewportPoint.x() >= 0 && viewportPoint.x() < viewport()->width());
    assert(viewportPoint.y() >= 0 && viewportPoint.y() < viewport()->height());

    // Maybe there is no need to do anything
    const double newScale = clipScale(offeredScale);
    if (!shouldRender(newScale, newModel, modelPoint, viewportPoint)) {
       return Scale;
    }

    // The part below is quite tricky: while working at the model plane scale,
    // we construct a "viewport" rectangle and position it so that it contains the
    // area of the model, which should be displayed in the real viewport at the
    // end. We do this because the "should-be-displayed" area might go beyond
    // the model plane bounds, in which case the model plane size would have to
    // be adjusted to include this area.
    //
    // Remember that <size in model plane> * <Scale> = <size in view plane>

    QSize viewportSizeOnModelPlane = viewportRect().size() / newScale;

    QPoint vpTopLeftOnModelPlane{
        modelPoint.x() - static_cast<int>(std::round(viewportPoint.x() / newScale)),
        modelPoint.y() - static_cast<int>(std::round(viewportPoint.y() / newScale))
    };

    QRect viewportOnModelPlane{vpTopLeftOnModelPlane, viewportSizeOnModelPlane};
    newModel |= viewportOnModelPlane;

    // At this point everything is ready for rendering and positioning

    // Set new model size
    ViewX1 = newModel.left();
    ViewY1 = newModel.top();
    ViewX2 = newModel.left() + newModel.width();
    ViewY2 = newModel.top() + newModel.height();

    Scale = newScale;
    resizeContents(static_cast<int>(std::round(newModel.width() * Scale)),
                   static_cast<int>(std::round(newModel.height() * Scale)));

    auto contentTopLeft = modelToContents(vpTopLeftOnModelPlane);
    setContentsPos(contentTopLeft.x(), contentTopLeft.y());

    viewport()->update();
    App->view->drawn = false;

    return Scale;
}

double Schematic::renderModel(const double offeredScale)
{
    const auto currentModelBounds = modelRect();
    return renderModel(offeredScale, currentModelBounds,
                       currentModelBounds.center(), viewportRect().center());
}

double Schematic::renderModel(const double offeredScale,const QPoint modelPoint, const QPoint viewportPoint)
{
    return renderModel(offeredScale, modelRect(), modelPoint, viewportPoint);
}