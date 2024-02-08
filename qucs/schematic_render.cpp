/** @file
 A set of methods to render schematic and to convert between viewport,
 contents and model coordinate systems.

 @c Schematic object has @e contents and @e viewport –– they are inherited
 from Q3ScrollView.

 @e Contents is basically a canvas to draw something on it. Its size is not
 fixed and may be changed anytime (@ref Q3ScrollView::resizeContents)

 @e Viewport is the visible part of the contents and at the same time it's
 the widget you actually see. Contents may be larger than the display and
 viewport allows us to scroll the contents and to observe parts of it.

 @e Model is not inhereted from Q3ScrollView and is not represented by any
 widget, it is actually entirely abstract/logical entity. It describes some
 abstract space where an "idea" of the schematic lives. All dimensions and
 sizes in model are @e reference dimensions and sizes. For example, when
 schematic has to be drawn in double scale (twice as large), model size
 multiplied by two is the contents size for double-scale schematic. Want
 to draw schematic in half-scale? Take model size and divide it by two,
 that would be the contents size for half-scale schematic. In other words
 contents is the @em projection of model and there is a connection between
 them: @code <size in model> * <Scale> = <size on contents> @endcode

 Because of this connection between model and contents every point
 on contents has a counterpart in model and vise-versa.

 Contents coordinates originate at top-left corner and grow to the right
 and downwards. Model coordinates also originate at top-left corner,
 but while contents top-left is always (0,0), model's top-left is not
 fixed and changes as user scrolls or occupies space with schematic
 elements. Think about model as of a rectangle somewhere in cartesian
 coordinate system.

 There is also viewport coordinates; they are relative to top left
 corner of the viewport widget.

 When you click on schematic, position of the click may be described in
  - viewport coordinates (relative to top left corner of what you see)
  - contents coordinates (relative to top left corner of contents, which
    may not be visible at them moment)
  - model coordinates

 There is a set of methods to convert between types of coordinates.

 The core of drawing a schematic –– or @e rendering –– is @ref Schematic::renderModel
 method. Use it when you need to show display changes applied to schematic, like:
  - schematic should be drawn in another scale
  - model size has changed because the view has been scrolled
 See method description for example usage.
*/
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
    // DO NOT alter model bounds or scale and DO NOT call resizeContens() outside
    // of this method. It may break the state and lead to hard-to-find bugs.
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