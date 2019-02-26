def generate_intrinsics():
    with open("python_syscalls.txt", "r") as fh:
        with open("syscalls_gen.txt", "w") as out_fh:
            for line in fh:
                line = line.strip()
                line_parts = line.split("func")
                syscall_name = line_parts[0].strip()
                syscall_args = line_parts[1].strip()

                args_in, args_out = syscall_args.split("->")

                if args_out == "i32":
                    out_str = "I32"
                elif args_out == "()":
                    out_str = "void"

                if args_in == "()":
                    log_str = "\"S - {}\"".format(syscall_name)
                    arg_str = ""
                elif args_in == "i32":
                    log_str = "\"S - {} - {{}}\", a".format(syscall_name)
                    arg_str = ", I32 a"
                elif args_in == "(i32, i32)":
                    log_str = "\"S - {} - {{}} {{}}\", a, b".format(syscall_name)
                    arg_str = ", I32 a, I32 b"
                elif args_in == "(i32, i32, i32)":
                    log_str = "\"S - {} - {{}} {{}} {{}}\", a, b, c".format(syscall_name)
                    arg_str = ", I32 a, I32 b, I32 c"
                elif args_in == "(i32, i32, i32, i32)":
                    log_str = "\"S - {} - {{}} {{}} {{}} {{}}\", a, b, c, d".format(syscall_name)
                    arg_str = ", I32 a, I32 b, I32 c, I32 d"
                else:
                    print("Unrecognised line: {}\n".format(line))
                    exit(1)

                output_string = [
                    "DEFINE_INTRINSIC_FUNCTION(emEnv, \"{}\", {}, {} {}) {{\n".format(syscall_name, out_str,
                                                                                     syscall_name, arg_str),
                    "\tutil::getLogger()->debug({});\n".format(log_str),
                    "\tthrowException(Runtime::ExceptionTypes::calledUnimplementedIntrinsic);\n"
                    "}\n\n"

                ]

                out_fh.write(" ".join(output_string))


if __name__ == "__main__":
    generate_intrinsics()