// Replace `import { defineConfig } from 'vitepress'` so we can use mermaid
import { withMermaid } from "vitepress-plugin-mermaid";
import footnote from "markdown-it-footnote";

// NB:  For more details, see <https://vitepress.dev/reference/site-config>
export default withMermaid({
  title: "CSE 398/498 SPE",
  description: "Software Performance Engineering Class Discussion Notes",
  markdown: {
    math: true,
    config: (md) => md.use(footnote),
  },
  appearance: false,
  outDir: "./dist",
  base: "/spe/",
  ignoreDeadLinks: true,
  // Change the folder where the vitepress cache goes, because it doesn't need
  // version control, but it's annoying when child folders need their own
  // .gitignores.
  cacheDir: "./.cache",
})
