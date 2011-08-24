
#ifndef __ui_base_layer_h__
#define __ui_base_layer_h__

#pragma once

#include <vector>

#include "base/memory/ref_counted.h"

#include "ui_gfx/rect.h"
#include "ui_gfx/transform.h"
#include "compositor.h"

class SkCanvas;

namespace ui
{

    class Compositor;
    class Texture;

    // Layer manages a texture, transform and a set of child Layers. Any View that
    // has enabled layers ends up creating a Layer to manage the texture.
    //
    // NOTE: unlike Views, each Layer does *not* own its children views. If you
    // delete a Layer and it has children, the parent of each child layer is set to
    // NULL, but the children are not deleted.
    class Layer
    {
    public:
        explicit Layer(Compositor* compositor);
        ~Layer();

        // Adds a new Layer to this Layer.
        void Add(Layer* child);

        // Removes a Layer from this Layer.
        void Remove(Layer* child);

        // Returns the child Layers.
        const std::vector<Layer*>& children() { return children_; }

        // The parent.
        const Layer* parent() const { return parent_; }
        Layer* parent() { return parent_; }

        // The transform, relative to the parent.
        void SetTransform(const gfx::Transform& transform);
        const gfx::Transform& transform() const { return transform_; }

        // The bounds, relative to the parent.
        void SetBounds(const gfx::Rect& bounds);
        const gfx::Rect& bounds() const { return bounds_; }

        // See description in View for details
        void SetFillsBoundsOpaquely(bool fills_bounds_opaquely);
        bool fills_bounds_opaquely() const { return fills_bounds_opaquely_; }

        const gfx::Rect& hole_rect() const {  return hole_rect_; }

        // The compositor.
        const Compositor* compositor() const { return compositor_; }
        Compositor* compositor() { return compositor_; }

        // Passing NULL will cause the layer to get a texture from its compositor.
        void SetTexture(Texture* texture);
        const ui::Texture* texture() const { return texture_.get(); }

        // Resets the canvas of the texture.
        void SetCanvas(const SkCanvas& canvas, const gfx::Point& origin);

        // Draws the layer with hole if hole is non empty.
        // hole looks like:
        //
        //  layer____________________________
        //  |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
        //  |xxxxxxxxxxxxx top xxxxxxxxxxxxxx|
        //  |________________________________|
        //  |xxxxx|                    |xxxxx|
        //  |xxxxx|      Hole Rect     |xxxxx|
        //  |left | (not composited)   |right|
        //  |_____|____________________|_____|
        //  |xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx|
        //  |xxxxxxxxxx bottom xxxxxxxxxxxxxx|
        //  |________________________________|
        //
        // Legend:
        //   composited area: x
        void Draw();

    private:
        // calls Texture::Draw only if the region to be drawn is non empty
        void DrawRegion(const ui::TextureDrawParams& params,
            const gfx::Rect& region_to_draw);

        // A hole in a layer is an area in the layer that does not get drawn
        // because this area is covered up with another layer which is known to be
        // opaque.
        // This method computes the dimension of the hole (if there is one)
        // based on whether one of its child nodes is always opaque.
        // Note: For simpicity's sake, currently a hole is only created if the child
        // view has no transfrom with respect to its parent.
        void RecomputeHole();

        Compositor* compositor_;

        scoped_refptr<Texture> texture_;

        Layer* parent_;

        std::vector<Layer*> children_;

        gfx::Transform transform_;

        gfx::Rect bounds_;

        bool fills_bounds_opaquely_;

        gfx::Rect hole_rect_;

        DISALLOW_COPY_AND_ASSIGN(Layer);
    };

} //namespace ui

#endif //__ui_base_layer_h__