import cv2
import numpy as np

np.random.seed(0)
path = "C:/Users/Administrator/Desktop/OpencvTestImg/imori_many.jpg"

img = cv2.imread(path)
H, W, C = img.shape

# 灰度化
gray = 0.2126 * img[..., 2] + 0.7152 * img[..., 1] + 0.0722 * img[..., 0]

# 识别区域
gt = np.array((47, 41, 129, 103), dtype=np.float32)

# 绘制边框
cv2.rectangle(img, (gt[0], gt[1]), (gt[2], gt[3]), (0, 255, 255), 1)


def iou(a, b):
    print(a)
    print(b)

    # 面积
    area_a = (a[2] - a[0]) * (a[3] - a[1])
    area_b = (b[2] - b[0]) * (b[3] - b[1])
    print("a的面积", area_a, 'b的面积：', area_b)

    # 左上角，左下角，右上角，右下角
    x1 = np.maximum(a[0], b[0])
    y1 = np.maximum(a[1], b[1])
    x2 = np.minimum(a[2], b[2])
    y2 = np.minimum(a[3], b[3])

    width = x2 - x1
    height = y2 - y1
    print("width", width)
    print("height", height)

    # 检测结果：DetectionResult
    # 基本事实：Ground Truth
    dr = width * height
    gt = area_a + area_b - dr
    iou = dr / gt

    print("dr", dr)
    print("gt", gt)
    print("iou", iou)
    return iou


def hog(gray):
    h, w = gray.shape

    gray = np.pad(gray, (1, 1), 'edge')

    gx = gray[1:h + 1, 2:] - gray[1:h + 1, :w]
    gy = gray[2:, 1:w + 1] - gray[:h, 1:w + 1]
    gx[gx == 0] = 0.000001

    mag = np.sqrt(gx ** 2 + gy ** 2)
    gra = np.arctan(gy / gx)
    gra[gra < 0] = np.pi / 2 + gra[gra < 0] + np.pi / 2

    gra_n = np.zeros_like(gra, dtype=np.int)

    d = np.pi / 9
    for i in range(9):
        gra_n[np.where((gra >= d * i) & (gra <= d * (i + 1)))] = i

    N = 8
    HH = h // N
    HW = w // N
    Hist = np.zeros((HH, HW, 9), dtype=np.float32)
    for y in range(HH):
        for x in range(HW):
            for j in range(N):
                for i in range(N):
                    Hist[y, x, gra_n[y * 4 + j, x * 4 + i]] += mag[y * 4 + j, x * 4 + i]

    ## Normalization
    C = 3
    eps = 1
    for y in range(HH):
        for x in range(HW):
            Hist[y, x] /= np.sqrt(np.sum(Hist[max(y - 1, 0):min(y + 2, HH), max(x - 1, 0):min(x + 2, HW)] ** 2) + eps)

    return Hist


def resize(img, h, w):
    _h, _w = img.shape
    ah = 1. * h / _h
    aw = 1. * w / _w
    y = np.arange(h).repeat(w).reshape(w, -1)
    x = np.tile(np.arange(w), (h, 1))
    y = (y / ah)
    x = (x / aw)

    ix = np.floor(x).astype(np.int32)
    iy = np.floor(y).astype(np.int32)
    ix = np.minimum(ix, _w - 2)
    iy = np.minimum(iy, _h - 2)

    dx = x - ix
    dy = y - iy

    out = (1 - dx) * (1 - dy) * img[iy, ix] + dx * (1 - dy) * img[iy, ix + 1] + (1 - dx) * dy * img[
        iy + 1, ix] + dx * dy * img[iy + 1, ix + 1]
    out[out > 255] = 255

    return out


class NN:
    def __init__(self, ind=2, w=64, w2=64, outd=1, lr=0.1):
        self.w1 = np.random.normal(0, 1, [ind, w])
        self.b1 = np.random.normal(0, 1, [w])
        self.w2 = np.random.normal(0, 1, [w, w2])
        self.b2 = np.random.normal(0, 1, [w2])
        self.wout = np.random.normal(0, 1, [w2, outd])
        self.bout = np.random.normal(0, 1, [outd])
        self.lr = lr

    def forward(self, x):
        self.z1 = x
        self.z2 = sigmoid(np.dot(self.z1, self.w1) + self.b1)
        self.z3 = sigmoid(np.dot(self.z2, self.w2) + self.b2)
        self.out = sigmoid(np.dot(self.z3, self.wout) + self.bout)
        return self.out

    def train(self, x, t):
        En = (self.out - t) * self.out * (1 - self.out)
        grad_wout = np.dot(self.z3.T, En)
        grad_bout = np.dot(np.ones([En.shape[0]]), En)
        self.wout -= self.lr * grad_wout
        self.bout -= self.lr * grad_bout

        # backpropagation inter layer
        grad_u2 = np.dot(En, self.wout.T) * self.z3 * (1 - self.z3)
        grad_w2 = np.dot(self.z2.T, grad_u2)
        grad_b2 = np.dot(np.ones([grad_u2.shape[0]]), grad_u2)
        self.w2 -= self.lr * grad_w2
        self.b2 -= self.lr * grad_b2

        grad_u1 = np.dot(grad_u2, self.w2.T) * self.z2 * (1 - self.z2)
        grad_w1 = np.dot(self.z1.T, grad_u1)
        grad_b1 = np.dot(np.ones([grad_u1.shape[0]]), grad_u1)
        self.w1 -= self.lr * grad_w1
        self.b1 -= self.lr * grad_b1


def sigmoid(x):
    return 1. / (1. + np.exp(-x))


if __name__ == "__main__":
    Crop_num = 200
    L = 60
    H_size = 32
    F_n = ((H_size // 8) ** 2) * 9

    db = np.zeros((Crop_num, F_n + 1))

    for i in range(Crop_num):
        x1 = np.random.randint(W - L)
        y1 = np.random.randint(H - L)
        x2 = x1 + L
        y2 = y1 + L
        crop = np.array((x1, y1, x2, y2))

        _iou = iou(gt, crop)

        if _iou >= 0.5:
            cv2.rectangle(img, (x1, y1), (x2, y2), (0, 0, 255), 1)
            label = 1
        else:
            cv2.rectangle(img, (x1, y1), (x2, y2), (255, 0, 0), 1)
            label = 0

        crop_area = gray[y1:y2, x1:x2]
        crop_area = resize(crop_area, H_size, H_size)
        _hog = hog(crop_area)

        db[i, :F_n] = _hog.ravel()
        db[i, -1] = label

    nn = NN(ind=F_n, lr=0.01)
    for i in range(10000):
        nn.forward(db[:, :F_n])
        nn.train(db[:, :F_n], db[:, -1][..., None])

    img2 = cv2.imread(path)
    H2, W2, C2 = img2.shape

    gray2 = 0.2126 * img2[..., 2] + 0.7152 * img2[..., 1] + 0.0722 * img2[..., 0]

    recs = np.array(((42, 42), (56, 56), (70, 70)), dtype=np.float32)

    detects = np.ndarray((0, 5), dtype=np.float32)

    # 滑动窗口
    for y in range(0, H2, 4):
        for x in range(0, W2, 4):
            for rec in recs:
                dh = int(rec[0] // 2)
                dw = int(rec[1] // 2)
                x1 = max(x - dw, 0)
                x2 = min(x + dw, W2)
                y1 = max(y - dh, 0)
                y2 = min(y + dh, H2)
                region = gray2[max(y - dh, 0):min(y + dh, H2), max(x - dw, 0):min(x + dw, W2)]
                region = resize(region, H_size, H_size)
                region_hog = hog(region).ravel()

                score = nn.forward(region_hog)
                if score >= 0.7:
                    cv2.rectangle(img2, (x1, y1), (x2, y2), (0, 0, 255), 1)
                    detects = np.vstack((detects, np.array((x1, y1, x2, y2, score))))

    print(detects)
    cv2.imshow("结果", img2)
    cv2.waitKey(0)
