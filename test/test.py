import os
import sys
import png

def check_result(testname, actual, ref):
    diff_limit = 5
    ref_w, ref_h, ref_pix, _ = png.Reader(filename=ref).read()
    actual_w, actual_h, actual_pix, _ = png.Reader(filename=actual).read()
    if not (ref_w == actual_w and ref_h == actual_h):
        print("ERROR {0}: Different image sizes (WxH). Reference image is \
                {1}x{2} and actual image is {3}x{4}. \
                Aborting test case".format(testname, ref_w, ref_h, actual_w, actual_h))
        return False
    rows_iter = zip(ref_pix, actual_pix)

    diff = 0
    for (ref_row, actual_row) in rows_iter:
        for i in range(0, ref_w):
            diff += abs(ref_row[i] - actual_row[i])

    if diff > diff_limit:
        print("FAIL {0} (diff = {1}): Too large of a difference between \
                the actual and reference images.".format(testname, diff))
        return False
    print("PASS: {0}".format(testname))
    return True

def ref_output_exists(target):
    f_ref = ref_output_path(target)
    if not os.path.isfile(f_ref):
        print("Missing reference output for {0}, \
                please run 'rake generate_ref_output' while on \
                the reference build branch".format(f_ref))
        return False
    return True

def ref_output_path(target):
    return "./test/ref_output/{0}-ref.png".format(target)

def scene_path(target):
    return "./scenes/{0}.txt".format(target)

def main():
    verbose = False
    if len(sys.argv) > 1 and sys.argv[1] == '-v':
        verbose = True
    to_test = []
    passed_tests = 0
    for filename in os.listdir("./scenes"):
        if filename.endswith(".txt"):
            test_name = filename[:-4]
            if ref_output_exists(test_name):
                to_test.append(test_name)
                print("Queueing test case: {0}".format(test_name))
    for case in to_test:
        target = "/tmp/{0}.png".format(case)
        print("Running test case: {0}".format(case))
        if verbose:
            os.system("./build/raytracer -o {0} < {1}".format(target, scene_path(case)))
        else:
            os.system("./build/raytracer -o {0} < {1} | echo -n".format(target, scene_path(case)))
        result = check_result(case, target, ref_output_path(case))
        if result:
            passed_tests += 1
    print("Testing finished {0}/{1} PASSES".format(passed_tests, len(to_test)))

if __name__ == '__main__':
    main()
