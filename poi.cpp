#include "poi.h"
#include "filters.h"

namespace cv {

static double C(const Image& image, int window_size, int x, int y, int dx, int dy){

    auto sum = 0.0;
    auto window_half = window_size / 2;

    for(int u = -window_half; u <= window_half; u++)
        for(int v = -window_half; v <= window_half; v++){
            auto tmp = image.get(x + u, y + v) - image.get(x + u + dx, y + v + dy);
            sum += tmp * tmp;
        }

    return sum;
}

double getDistance(int x1, int y1, int x2, int y2){
    auto x = x1 - x2;
    auto y = y1 - y2;
    return sqrt(x * x + y * y);

}

Image Moravec(const Image& image, int window_size){
    auto H = image.getHeight();
    auto W = image.getWidth();
    Image result(H, W);

    for(int i = 0; i < H; i++)
        for(int j = 0; j < W; j++){
            auto s = std::numeric_limits<double>::max();
            for(int dx = -1; dx <= 1; dx++)
                for(int dy = -1; dy <= 1; dy++)
                    if(dx != 0 && dy != 0){
                        auto c = C(image, window_size, i, j, dx, dy);
                        s = std::min(s, c);
                    }
            result.set(i, j, s);
        }
    return result;
}

Image Harris(const Image& image, int window_size, double k){
    auto sobel_Dx = SobelDx(image);
    auto sobel_Dy = SobelDy(image);
    auto H = image.getHeight();
    auto W = image.getWidth();

    Image a(H, W);
    Image b(H, W);
    Image c(H, W);

    auto window_half = window_size / 2;

    for(int i = 0; i < H; i++)
        for(int j = 0; j < W; j++){
            auto sum_a = .0, sum_b = .0, sum_c = .0;
            for(int u = -window_half; u <= window_half; u++)
                for(int v = -window_half; v <= window_half; v++){
                    auto x_grad = sobel_Dx.get(i + u, j + v);
                    auto y_grad = sobel_Dy.get(i + u, j + v);
                    sum_a += x_grad * x_grad;
                    sum_b += x_grad * y_grad;
                    sum_c += y_grad * y_grad;
                }
            a.set(i, j, sum_a);
            b.set(i, j, sum_b);
            c.set(i, j, sum_c);
        }

    Image result(H, W);

    for(int i = 0; i < H; i++)
        for(int j = 0; j < W; j++){
            auto det = a.get(i, j) * c.get(i, j) - b.get(i, j) * b.get(i, j);
            auto trace = a.get(i, j) + c.get(i, j);
            auto harris = det - k * trace * trace;
            result.set(i, j, harris);
        }

    return result;
}

IPoints getPoi(const Image& image, double threshold, int p_size){
    auto p_half = p_size / 2;
    auto H = image.getHeight();
    auto W = image.getWidth();

    IPoints points;

    for(int i = 0; i < H; i++)
        for(int j = 0; j < W; j++){
            bool isMax = true;
            for(int px = - p_half; px <= p_half && isMax; px++)
                for(int py = - p_half; py <= p_half && isMax; py++)
                    if(px != 0 || py != 0)
                        isMax = image.get(i, j) > image.get(i + px, j + py);
            if(isMax && image.get(i, j) > threshold)
                points.emplace_back(i, j, image.get(i, j));
        }

    return points;
}

IPoints filterPoints(const IPoints& points, int target_quantity){
    IPoints filter_points(points);

    int r = 1;

    while(filter_points.size() > target_quantity){
        for(int i = 0; i < filter_points.size(); i++)
            for(int j = 0; j < filter_points.size(); j++){
                if(i == filter_points.size()) i--;
                auto dist = getDistance(std::get<0>(filter_points[i]), std::get<1>(filter_points[i]),
                                    std::get<0>(filter_points[j]), std::get<1>(filter_points[j]));
                if(dist < r && std::get<2>(filter_points[i]) < std::get<2>(filter_points[j]))
                    filter_points.erase(filter_points.begin() + i);
            }
        r++;
    }
    return filter_points;
}

}
