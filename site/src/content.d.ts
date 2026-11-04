declare module '*.md' {
  const page: import('./docs').Page
  export default page
}
