import { RootState } from "root@redux/store";
declare module "react-redux" {
    interface DefaultRootState extends RootState {}
}
