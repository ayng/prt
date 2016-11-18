import os
import png
import itertools

def main():
    to_test = []
    for f in os.listdir("./scenes"):
        if f.endswith(".txt"):
            test_name = f[:-4]
            if ref_output_exists(test_name):
                to_test.append(test_name)
                print("Queueing test case: {0}".format(test_name))
    
    for case in to_test:
        target = "/tmp/{0}.png".format(case)
        print("Running test case: {0}".format(case))
        os.system("./build/raytracer -o {0} < {1}".format(target, scene_path(case)))
        check_output(case, target, ref_output_path(case))

def check_result(testname, actual, ref):
    ref_w, ref_h, ref_pix, _ = png.Reader(filename=ref).read()
    actual_w, actual_h, actual_pix = png.Reader(filename=actual).read()
    if not (ref_w == actual_w and ref_h == actual_h):
        print("ERROR {0}: Different image sizes (WxH). Reference image is \
                {1}x{2} and actual image is {3}x{4}. Aborting test case".format(case, ref_w, ref_h, actual_w, actual_h))
        return
    rows_iter = itertools.izip(ref_pix, actual_pix)

    diff = 0
    for (ref_row, actual_row) in rows_iter:
        for i in range(0, ref_w):
            diff += abs(ref_row[i] - actual_row[i])

    if diff > MAX_DIFF:
        print("FAIL {0}: Too large of a difference between the actual and reference images.".format(case))
    else
        print("PASS: {0}".format(case))
        

def ref_output_exists(target):
    f_ref = ref_output_path(target)
    if not os.path.isfile(f_ref):
        print("Missing reference output for {0}, please run 'rake generate_ref_output' while on the reference build branch".format(f_ref))
        return False
    return True

def ref_output_path(target):
    return "./test/ref_output/{0}-ref.png".format(target)

def scene_path(target):
    return "./scenes/{0}.txt".format(target)

if __name__ == '__main__':
    main()

